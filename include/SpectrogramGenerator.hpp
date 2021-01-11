#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#include "GeneratorConfiguration.hpp"
#include "Decoder.hpp"
#include "FFT.hpp"
#include "ImageGenerator.hpp"

typedef class SpectrogramGenerator
{
  Decoder *decoder_;
  ImageGenerator *picture_;
  Fft_samples *transformation_;

  sample_fmt *data_; // array containing raw audio data
  int data_size_;    // number of samples

public:
  SpectrogramGenerator(const GeneratorConfiguration *cfg);
  ~SpectrogramGenerator();

  void openFile(std::string file_name);
  void setupDecoder();
  void decodeAudioFile();

  void processSamples();

  void plotSpectrogram();
} SpecGen;

#endif
