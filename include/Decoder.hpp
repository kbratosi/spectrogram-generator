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

  void initCodecContext();
  void initSwrContext();
  void initPacket();
  void initFrame();

  void allocateMemory(sample_fmt **data);
  void reallocateMemory(sample_fmt **data, int new_sample_capacity);
  void addOverlapPrefix(sample_fmt **data, int *data_size);
  void readFile(sample_fmt **data, int *data_size);
  void addOverlapSuffix(sample_fmt **data, int *data_size);

  const char *avMakeError(int errnum);

public:
  Decoder(const GeneratorConfiguration *cfg);
  ~Decoder();
  // open audio file and retrieve information from its header
  void openFile(const char *file_name);
  // initialize fields and structures necessary in decoding process
  void setup();
  // retrieve audio data from file, prepare data stream for fft transforms
  void decode(sample_fmt **data, int *data_size);
};

#endif