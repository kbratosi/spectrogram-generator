#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#include "Decoder.hpp"
#include "fft.hpp"

typedef class SpectrogramGenerator
{
  Decoder decoder_;
  Fft_samples *transformation_;

public:              // temporary - change later!
  sample_fmt *data_; // array containing raw audio data
  int data_size_;    // number of samples
  int sample_rate_;  // sampling frequency of decoded file

public:
  SpectrogramGenerator(const int sample_rate, const uint inputSamples);
  ~SpectrogramGenerator();
  int setupDecoder(const char *file_name);
  int decodeAudioFile();

  void processSamples();

  //~
} SpecGen;

#endif