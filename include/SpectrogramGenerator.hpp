#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#include "ConfigReader.hpp"
#include "Decoder.hpp"
#include "fft.hpp"
#include "drawImg.hpp"

typedef class SpectrogramGenerator
{
  Decoder decoder_;
  SpecImage *picture_;
  Fft_samples *transformation_;

public:                   // temporary - change later!
  sample_fmt *data_;      // array containing raw audio data
  int data_size_;         // number of samples
  int out_sample_rate_;   // sampling frequency of decoded file

public:
  SpectrogramGenerator(const GeneratorConfiguration *cfg);
  ~SpectrogramGenerator();
  int setupDecoder(const char *file_name);
  int decodeAudioFile();

  void processSamples();
  void plotSpectrogram();

  //~
} SpecGen;

#endif