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

typedef uint16_t sample_fmt;

typedef class SpectrogramGenerator {
  /* parameters */
  public:
  sample_fmt *data_;
  int data_size_; // n_samples * sizeof (BitDepth)
  int sample_rate_;

  public:
  SpectrogramGenerator(const int sample_rate);
  int decodeAudioFile(const char *file_name);
  //~
} SpecGen;

#endif