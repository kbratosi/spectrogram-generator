#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

typedef class SpectrogramGenerator {
  /* parameters */
  double *data_;
  int data_size_;
  int sample_rate_;

  public:
  SpectrogramGenerator(const int sample_rate);
  int decodeAudioFile(const char *file_name);
  //~
} SpecGen;

#endif