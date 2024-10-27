/*
 *
 * Created by rgctlln on 17.04.2023
 *
 */

#include "ffmpeg_functions.h"

#include "avconvertfields.h"
#include "return_codes.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>

static uint8_t open_file(const char *file, AVFormatContext **av_fmt)
{
	int val = avformat_open_input(av_fmt, file, NULL, NULL);
	if (val < 0)
	{
		switch (val)
		{
		case AVERROR(ENOMEM):
			fputs("ERROR: Unable to allocate memory", stderr);
			return ERROR_NOTENOUGH_MEMORY;
		case AVERROR(EIO):
			fputs("ERROR: Unable to open file", stderr);
			return ERROR_FORMAT_INVALID;
		case AVERROR(ENOENT):
			fputs("ERROR: Unable to find file", stderr);
			return ERROR_CANNOT_OPEN_FILE;
		case AVERROR(EACCES):
			fputs("ERROR: Unable to get access to the file", stderr);
			return ERROR_CANNOT_OPEN_FILE;
		default:
			fputs("ERROR: Unknown", stderr);
			return ERROR_UNKNOWN;
		}
	}
	if (avformat_find_stream_info(*av_fmt, NULL) < 0)
	{
		fputs("ERROR: Unsupported type of input file", stderr);
		return ERROR_UNSUPPORTED;
	}
	return SUCCESS;
}

void close_files(AVFormatContext **file1, AVFormatContext **file2)
{
	if (*file1)
	{
		avformat_close_input(file1);
		*file1 = NULL;
	}
	if (*file2)
	{
		avformat_close_input(file2);
		*file2 = NULL;
	}
}

static size_t find_audio_stream_index(AVFormatContext *format_context)
{
	size_t idx = -1;
	for (size_t i = 0; i < format_context->nb_streams; i++)
	{
		if (format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			idx = i;
			break;
		}
	}
	return idx;
}

static uint8_t check_codec(const AVCodecContext *codec, AVFormatContext **file1, AVFormatContext **file2)
{
	if (codec == NULL)
	{
		close_files(file1, file2);
		return ERROR_UNSUPPORTED;
	}
	return SUCCESS;
}

void fill_buffer(
	double **audio_buffer,
	AVCodecContext *codec_context,
	AVPacket *packet,
	AVFrame *frame,
	size_t *buffer_size,
	size_t *buffer_capacity,
	AVStream *stream,
	AVFormatContext *file,
	uint8_t idx,
	SwrContext **swr_context,
	uint8_t *error_code)
{
	int32_t cnt;
	while (av_read_frame(file, packet) >= 0)
	{
		if (packet->stream_index != stream->index)
			continue;
		if (avcodec_send_packet(codec_context, packet) != 0)
		{
			av_packet_unref(packet);
			continue;
		}
		while (avcodec_receive_frame(codec_context, frame) == 0)
		{
			uint8_t **converted_samples = NULL;
			if (av_samples_alloc_array_and_samples(&converted_samples, NULL, codec_context->ch_layout.nb_channels, frame->nb_samples, AV_SAMPLE_FMT_DBLP, 0) < 0)
			{
				fputs("ERROR: Unable to allocate array and samples", stderr);
				*error_code = ERROR_NOTENOUGH_MEMORY;
				return;
			}
			cnt = swr_convert(*swr_context, converted_samples, frame->nb_samples, (const uint8_t **)frame->data, frame->nb_samples);
			if (cnt < 0)
			{
				fputs("ERROR: Unable to covert", stderr);
				*error_code = ERROR_UNSUPPORTED;
				return;
			}
			if (*buffer_size + cnt >= *buffer_capacity)
			{
				*buffer_capacity *= 2;
				*audio_buffer = realloc(*audio_buffer, *buffer_capacity * sizeof(double));
				if (*audio_buffer == NULL)
				{
					av_freep(&converted_samples[0]);
					av_freep(&converted_samples);
					*error_code = ERROR_NOTENOUGH_MEMORY;
					return;
				}
			}
			double *buffer = (double *)converted_samples[idx];
			memcpy(*audio_buffer + *buffer_size, buffer, cnt * sizeof(double));
			*buffer_size += cnt;
			av_freep(&converted_samples[0]);
			av_freep(&converted_samples);
		}
		av_packet_unref(packet);
	}
}

void allocate_file(AVConvertFields *fields, AVConvertFields *fields_2, uint8_t *error_code, char *arr, int16_t *audio_stream_index, bool flag)
{
	uint8_t temp = open_file(arr, &fields->file);
	if (temp != 0)
	{
		fputs("ERROR: Unable to open file", stderr);
		*error_code = temp;
		return;
	}
	*audio_stream_index = find_audio_stream_index(fields->file);
	if (*audio_stream_index == -1)
	{
		fputs("ERROR: Unable to find audio stream", stderr);
		*error_code = ERROR_FORMAT_INVALID;
		return;
	}
	fields->stream = fields->file->streams[*audio_stream_index];
	fields->codec = avcodec_find_decoder(fields->stream->codecpar->codec_id);
	if (fields->codec == NULL)
	{
		*error_code = ERROR_UNSUPPORTED;
		fputs("ERROR: Unable to create Codec", stderr);
		return;
	}
	uint8_t checker_for_codec = check_codec((const AVCodecContext *)fields->codec, &fields->file, &fields_2->file);
	if (checker_for_codec != 0)
	{
		*error_code = checker_for_codec;
		fputs("ERROR: Unable to allocate memory for Codec", stderr);
		return;
	}
	fields->codec_context = avcodec_alloc_context3(fields->codec);
	if (fields->codec_context == NULL)
	{
		*error_code = ERROR_UNSUPPORTED;
		fputs("ERROR: Unable to create CodecContext", stderr);
		return;
	}
	int val = avcodec_parameters_to_context(fields->codec_context, fields->file->streams[*audio_stream_index]->codecpar);
	if (val < 0)
	{
		fputs("ERROR: Unable to copy Codec parameteres", stderr);
		*error_code = ERROR_UNSUPPORTED;
		return;
	}
	uint16_t audio_streams_cnt = fields->codec_context->ch_layout.nb_channels;
	if (audio_streams_cnt != 2 && flag)
	{
		fputs("ERROR: Wrong number of audio channels", stderr);
		*error_code = ERROR_FORMAT_INVALID;
		return;
	}
	uint8_t checker_for_codec_context = check_codec(fields->codec_context, &fields->file, &fields_2->file);
	if (checker_for_codec_context != 0)
	{
		fputs("ERROR: Unable to allocate memory for CodecContext", stderr);
		*error_code = checker_for_codec_context;
		return;
	}
	if (avcodec_open2(fields->codec_context, fields->codec, NULL) < 0)
	{
		fputs("ERROR: Unable to open codec", stderr);
		*error_code = ERROR_DATA_INVALID;
		return;
	}
}

void initialize(AVConvertFields *fields)
{
	fields->file = NULL;
	fields->codec = NULL;
	fields->codec_context = NULL;
	fields->stream = NULL;
}

void init_swr(SwrContext **swr_context, AVConvertFields fields_1, uint8_t *error_code, int32_t smp_rate)
{
	int32_t target = (smp_rate == -1) ? fields_1.codec_context->sample_rate : smp_rate;
	if (swr_alloc_set_opts2(
			swr_context,
			&fields_1.codec_context->ch_layout,
			AV_SAMPLE_FMT_DBLP,
			target,
			&fields_1.codec_context->ch_layout,
			fields_1.codec_context->sample_fmt,
			fields_1.codec_context->sample_rate,
			0,
			NULL) < 0)
	{
		fputs("ERROR: Unable to allocate SWRContext", stderr);
		*error_code = ERROR_NOTENOUGH_MEMORY;
	}
	if (swr_init(*swr_context) < 0)
	{
		fputs("ERROR: Unable to allocate the resampler context", stderr);
		*error_code = ERROR_UNSUPPORTED;
		return;
	}
}

static void check_audio_format(uint8_t *error_code, AVCodecContext *codec_ctx)
{
	enum AVCodecID codec_id = codec_ctx->codec_id;
	if (codec_id != AV_CODEC_ID_OPUS && codec_id != AV_CODEC_ID_FLAC && codec_id != AV_CODEC_ID_MP2 &&
		codec_id != AV_CODEC_ID_MP3 && codec_id != AV_CODEC_ID_AAC)
	{
		*error_code = ERROR_FORMAT_INVALID;
		return;
	}
	*error_code = SUCCESS;
}

void initialize_audio_processing(AVConvertFields *fields, char *filename, uint8_t *error_code, int16_t *audio_stream_index, bool seek)
{
	allocate_file(fields, fields, error_code, filename, audio_stream_index, seek);
	if (*error_code != SUCCESS)
	{
		fputs("ERROR: Unable to allocate file", stderr);
		return;
	}
	check_audio_format(error_code, fields->codec_context);
	if (*error_code != SUCCESS)
	{
		fputs("ERROR: Unsupported audio format", stderr);
		return;
	}
}

void check_packet_and_frame(AVPacket *packet, AVFrame *frame, uint8_t *error_code)
{
	if (!packet || !frame)
	{
		fputs("ERROR: Unable to allocate memory for audio buffers or AV structures", stderr);
		*error_code = ERROR_NOTENOUGH_MEMORY;
	}
}
// Перевод на новую строку, просто гит ее затирает