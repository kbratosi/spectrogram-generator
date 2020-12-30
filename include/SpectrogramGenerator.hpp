#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#ifdef __cplusplus
extern "C"
{
  #include <libavutil/opt.h>
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswresample/swresample.h>
}
#endif

typedef class SpectrogramGenerator {
  /* parameters */
  public:
  double *data_;
  int data_size_;
  int sample_rate_;

  public:
  SpectrogramGenerator(const int sample_rate);
  int decodeAudioFile(const char *file_name);
  //~
} SpecGen;

#endif