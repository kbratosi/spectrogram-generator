#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#include "Decoder.hpp"

typedef class SpectrogramGenerator {
  Decoder decoder_;

  public: // temporary - change later!
  sample_fmt *data_;  // array containing raw audio data
  int data_size_;     // number of samples * sizeof(sample_fmt)
  int sample_rate_;   // sampling frequency of decoded file

  public:
  SpectrogramGenerator(const int sample_rate);
  ~SpectrogramGenerator();
  int setupDecoder(const char *file_name);
  int decodeAudioFile();
  //~
} SpecGen;

#endif