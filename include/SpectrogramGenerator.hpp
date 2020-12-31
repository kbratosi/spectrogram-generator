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

class Decoder {
  AVFormatContext *av_format_ctx_;
  AVCodecContext *av_codec_ctx_;
  int audio_stream_index_;
  SwrContext *swr_;
  AVFrame *av_frame_;
  AVPacket *av_packet_;

  public:
  Decoder();
  ~Decoder();
  int setup(const char *file_name, const int out_sample_rate);
  int readFile(sample_fmt **data, int *data_size);
};

typedef class SpectrogramGenerator {
  Decoder decoder_;
  public:
  sample_fmt *data_;
  int data_size_; // n_samples * sizeof (BitDepth)
  int sample_rate_;

  public:
  SpectrogramGenerator(const int sample_rate);
  ~SpectrogramGenerator();
  int decodeAudioFile(const char *file_name);
  //~
} SpecGen;

#endif