#include "avconvertfields.h"
#include "ffmpeg_functions.h"
#include "fftw3_functions.h"
#include "return_codes.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>

#include <fftw3.h>
#include <stdbool.h>
#include <stdio.h>

static void pad_buffer(double **audio_buffer, size_t target_size, size_t size, uint8_t *error_code)
{
	double *new_buffer = realloc(*audio_buffer, target_size * sizeof(double));
	if (new_buffer == NULL)
	{
		fputs("ERROR: Failed to allocate memory", stderr);
		*error_code = ERROR_NOTENOUGH_MEMORY;
		return;
	}
	*audio_buffer = new_buffer;
	memset(*audio_buffer + size, 0, (target_size - size) * sizeof(double));
}

static void calculate_and_print_correlation(double *audio_buffer1, double *audio_buffer2, size_t max_size, int32_t smp_rate, uint8_t *error_code)
{
	int32_t delta = correlation_fftw(audio_buffer1, audio_buffer2, max_size, error_code);
	if (*error_code != SUCCESS)
	{
		return;
	}
	int delta_time = (int)(1000 * delta / smp_rate);
	printf("delta: %i samples\nsample rate: %i Hz\ndelta time: %i ms\n", delta, smp_rate, delta_time);
	*error_code = SUCCESS;
}

static void
	handle_codec_behavior(AVConvertFields *fields1, AVConvertFields *fields2, AVPacket **packet, AVFrame **frame, int16_t *audio_stream_index, char *filename, uint8_t *error_code)
{
	if (fields1->codec_context->codec_id != AV_CODEC_ID_OPUS)
	{
		av_seek_frame(fields1->file, *audio_stream_index, 0, AVSEEK_FLAG_BACKWARD);
	}
	else
	{
		av_packet_free(packet);
		av_frame_free(frame);
		if (fields1->codec_context)
			avcodec_free_context(&fields1->codec_context);
		if (fields2->codec_context)
			avcodec_free_context(&fields2->codec_context);
		*frame = av_frame_alloc();
		*packet = av_packet_alloc();
		close_files(&fields1->file, &fields2->file);
		allocate_file(fields1, fields2, error_code, filename, audio_stream_index, true);
		if (!*packet || !*frame)
		{
			fputs("ERROR: Unable to allocate memory for AV structures", stderr);
			*error_code = ERROR_NOTENOUGH_MEMORY;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3)
	{
		fputs("ERROR: Invalid amount of arguments", stderr);
		return ERROR_ARGUMENTS_INVALID;
	}
	av_log_set_level(AV_LOG_QUIET);
	AVConvertFields fields_1;
	AVConvertFields fields_2;
	initialize(&fields_1);
	initialize(&fields_2);
	AVFrame *frame = NULL;
	AVPacket *packet = NULL;
	frame = av_frame_alloc();
	packet = av_packet_alloc();
	double *audio_buffer1 = NULL, *audio_buffer2 = NULL;
	uint8_t error_code = SUCCESS;
	check_packet_and_frame(packet, frame, &error_code);
	if (error_code != SUCCESS)
	{
		goto cleanup;
	}
	SwrContext *swr_context = swr_alloc();
	if (!swr_context)
	{
		swr_free(&swr_context);
		error_code = ERROR_NOTENOUGH_MEMORY;
		goto cleanup;
	}
	int16_t audio_stream_index = -1;
	if (argc == 2)
	{
		initialize_audio_processing(&fields_1, argv[1], &error_code, &audio_stream_index, true);
	}
	else
	{
		initialize_audio_processing(&fields_1, argv[1], &error_code, &audio_stream_index, false);
	}
	if (error_code != SUCCESS)
	{
		goto cleanup;
	}
	size_t buffer_size1 = 0, buffer_size2 = 0;
	size_t buffer_capacity1 = 32768, buffer_capacity2 = 32768;
	audio_buffer1 = malloc(buffer_capacity1 * sizeof(double));
	audio_buffer2 = malloc(buffer_capacity2 * sizeof(double));
	int32_t smp_rate = fields_1.codec_context->sample_rate;
	int16_t audio_stream_idx2 = -1;
	if (argc == 3)
	{
		initialize_audio_processing(&fields_2, argv[2], &error_code, &audio_stream_idx2, false);
		if (error_code != SUCCESS)
		{
			goto cleanup;
		}
		smp_rate = fields_1.codec_context->sample_rate >= fields_2.codec_context->sample_rate
					 ? fields_1.codec_context->sample_rate
					 : fields_2.codec_context->sample_rate;
	}
	init_swr(&swr_context, fields_1, &error_code, smp_rate);
	if (error_code != SUCCESS)
	{
		goto cleanup;
	}
	fill_buffer(
		&audio_buffer1,
		fields_1.codec_context,
		packet,
		frame,
		&buffer_size1,
		&buffer_capacity1,
		fields_1.stream,
		fields_1.file,
		0,
		&swr_context,
		&error_code);
	if (error_code != SUCCESS)
	{
		goto cleanup;
	}
	if (argc == 2)
	{
		handle_codec_behavior(&fields_1, &fields_2, &packet, &frame, &audio_stream_index, argv[1], &error_code);
		if (error_code != SUCCESS)
		{
			goto cleanup;
		}
		fill_buffer(
			&audio_buffer2,
			fields_1.codec_context,
			packet,
			frame,
			&buffer_size2,
			&buffer_capacity2,
			fields_1.stream,
			fields_1.file,
			1,
			&swr_context,
			&error_code);
		if (error_code != SUCCESS)
		{
			goto cleanup;
		}
	}
	else
	{
		init_swr(&swr_context, fields_2, &error_code, smp_rate);
		if (error_code != SUCCESS)
		{
			goto cleanup;
		}
		fill_buffer(
			&audio_buffer2,
			fields_2.codec_context,
			packet,
			frame,
			&buffer_size2,
			&buffer_capacity2,
			fields_2.stream,
			fields_2.file,
			0,
			&swr_context,
			&error_code);
		if (error_code != SUCCESS)
		{
			goto cleanup;
		}
	}
	size_t max_size = buffer_size1 > buffer_size2 ? buffer_size1 : buffer_size2;
	if (argc == 3)
	{
		if (buffer_size1 > buffer_size2)
		{
			pad_buffer(&audio_buffer2, buffer_size1, buffer_size2, &error_code);
		}
		else if (buffer_size1 < buffer_size2)
		{
			pad_buffer(&audio_buffer1, buffer_size2, buffer_size1, &error_code);
		}
		if (error_code == ERROR_NOTENOUGH_MEMORY)
		{
			goto cleanup;
		}
	}
	calculate_and_print_correlation(audio_buffer1, audio_buffer2, max_size, smp_rate, &error_code);
	goto cleanup;
cleanup:
	if (packet)
		av_packet_free(&packet);
	if (frame)
		av_frame_free(&frame);
	if (fields_1.codec_context)
		avcodec_free_context(&fields_1.codec_context);
	if (fields_2.codec_context)
		avcodec_free_context(&fields_2.codec_context);
	if (audio_buffer1)
		free(audio_buffer1);
	if (audio_buffer2)
		free(audio_buffer2);
	close_files(&fields_1.file, &fields_2.file);
	return error_code;
}
// Перевод на новую строку, просто гит ее затирает