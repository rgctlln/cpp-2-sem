/*
 *
 * Created by rgctlln on 26.04.2023
 *
 */

#ifndef CT_C24_LW_LIBRARIES_RGCTLLN_AVCONVERTFIELDS_H
#define CT_C24_LW_LIBRARIES_RGCTLLN_AVCONVERTFIELDS_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>

typedef struct AVConvertFields
{
	AVFormatContext *file;
	const AVCodec *codec;
	AVCodecContext *codec_context;
	AVStream *stream;
} AVConvertFields;

#endif
// Перевод на новую строку, просто гит ее затирает