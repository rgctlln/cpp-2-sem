/*
 *
 * Created by rgctlln on 17.04.2023
 *
 */

#ifndef FFMPEG_FUNCTIONS_H
#define FFMPEG_FUNCTIONS_H

#include "avconvertfields.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>

#include <stdbool.h>

static uint8_t open_file(const char *file, AVFormatContext **av_fmt);

void close_files(AVFormatContext **file1, AVFormatContext **file2);

static size_t find_audio_stream_index(AVFormatContext *format_context);

static uint8_t check_codec(const AVCodecContext *codec, AVFormatContext **file1, AVFormatContext **file2);

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
	uint8_t *error_code);

void allocate_file(AVConvertFields *fields, AVConvertFields *fields_2, uint8_t *error_code, char *arr, int16_t *audio_stream_index, bool flag);

void initialize(AVConvertFields *fields);

void init_swr(SwrContext **swr_context, AVConvertFields fields_1, uint8_t *error_code, int32_t smp_rate);

static void check_audio_format(uint8_t *error_code, AVCodecContext *codec_ctx);

void initialize_audio_processing(AVConvertFields *fields, char *filename, uint8_t *error_code, int16_t *audio_stream_index, bool seek);

void check_packet_and_frame(AVPacket *packet, AVFrame *frame, uint8_t *error_code);
#endif
// Перевод на новую строку, просто гит ее затирает