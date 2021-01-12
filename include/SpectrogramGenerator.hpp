#ifndef SPECTROGRAMGENERATOR_HPP
#define SPECTROGRAMGENERATOR_HPP

#include "GeneratorConfiguration.hpp"
#include "Decoder.hpp"
#include "ImageGenerator.hpp"
#include "DataComputer.hpp"

///Decode and plot spectrograms from audio files
typedef class SpectrogramGenerator
{
  Decoder *decoder_;
  ImageGenerator *picture_;
  DataComputer *transformation_;

  sample_fmt *data_; // array containing raw audio data
  int data_size_;    // number of samples

public:
    ///@param cfg structure holding parameters required during spectrogram generation process
  ///@see GeneratorConfiguration
  SpectrogramGenerator(const GeneratorConfiguration *cfg);
  ~SpectrogramGenerator();

  void openFile(std::string file_name);
  /// wrapper for Decoder::setup()
  void setupDecoder();
  /// wrapper for Decoder::decode()
  void decodeAudioFile();
  ///wrapper for Decoder::processSamples()
  void processSamples();
  ///wrapper for ImageGenerator::createImage()
  void plotSpectrogram();
} SpecGen;

#endif
