#ifndef DECODER_HPP
#define DECODER_HPP

#ifdef __cplusplus
extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}
#endif

#include "ConfigReader.hpp"

#include <iostream>

typedef float sample_fmt;           // sample format - bit depth & int/float coding

class Decoder
{
  AVFormatContext *av_format_ctx_;  // file information
  AVCodecContext *av_codec_ctx_;    // codec information
  SwrContext *swr_;                 // resampler information
  AVPacket *av_packet_;             // compressed data
  AVFrame *av_frame_;               // raw audio
  int audio_stream_index_;

  const float IN_FRAME_COUNT;
  const float DELTA_FRAME;
  const int OUT_SAMPLE_RATE;

  const int MONO = 1;
  const int FRAME_ALLOC_UNIT = 4000000;

  void initFormatContext(const char *file_name);
  void initCodecContext();
  int initSwrContext(const int out_sample_rate);

  const char *avMakeError(int errnum);

public:
  Decoder(const GeneratorConfiguration *cfg);
  ~Decoder();
  void openFile(const char *file_name);
  int setup(const char *file_name, const int out_sample_rate);
  int readFile(sample_fmt **data, int *data_size);
};

#endif