/**
 * @file Decoder.hpp
 * @author Konrad Bratosiewicz
 * @brief File containing Decoder class
 * @version 1.0
 * @date 2021-01-12
 */

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

#include "GeneratorConfiguration.hpp"

typedef float SampleFormat; // sample format - bit depth & int/float coding

/// Class used for:
/// * opening an encoded audio file
/// * retrieving information about its contents
/// * decoding encoded audio packets to raw data
/// * saving it to a specified buffer.
/// Uses ffmpeg (libav).
class Decoder
{
  AVFormatContext *av_format_ctx_; // file information
  AVCodecContext *av_codec_ctx_;   // codec information
  SwrContext *swr_;                // resampler information
  AVPacket *av_packet_;            // compressed data
  AVFrame *av_frame_;              // raw audio
  int audio_stream_index_;
  //
  const float IN_FRAME_COUNT; //
  const float DELTA_FRAME;    // constants aquired from configuration
  const int OUT_SAMPLE_RATE;  //
  //
  const int MONO = 1;                   // number of channels of decoded audio stream
  const int FRAME_ALLOC_UNIT = 4000000; // number of av_frames allocated in single block of memory
  //
  void initCodecContext();
  void initSwrContext();
  void initPacket();
  void initFrame();
  //
  void allocateMemory(SampleFormat **data);
  void reallocateMemory(SampleFormat **data, int new_sample_capacity);
  void addOverlapPrefix(SampleFormat **data, int *data_size);
  void readFile(SampleFormat **data, int *data_size);
  void addOverlapSuffix(SampleFormat **data, int *data_size);
  //
  const char *avMakeError(int errnum);

public:
  ///@param cfg structure holding parameters required during spectrogram generation process
  ///@see GeneratorConfiguration
  Decoder(const GeneratorConfiguration *cfg);
  ~Decoder();
  //
  AVFormatContext *getAVFormatCtx();
  AVCodecContext *getAVCodecCtx();
  SwrContext *getSwr();
  AVPacket *getAVPacket();
  AVFrame *getAVFrame();
  //
  /// open audio file and retrieve information from its header
  void openFile(std::string file_name);

  /// initialize fields and structures necessary in decoding process
  void setup();

  /// retrieve audio data from file, prepare data stream for fft transforms
  ///@param data_size pointer to field storing number of samples
  ///@param data pointer to array storing raw audio signal
  void decode(SampleFormat **data, int *data_size);
};

#endif