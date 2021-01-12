/**
 * @file SpectrogramGenerator.cpp
 * @author Konrad Bratosiewicz
 * @brief File containing member functions of SpectrogramGenerator
 * @version 1.0
 * @date 2021-01-12
 */

#include "SpectrogramGenerator.hpp"

SpectrogramGenerator::SpectrogramGenerator(const GeneratorConfiguration *cfg)
{
  data_ = nullptr;
  data_size_ = 0;

  decoder_ = new Decoder(cfg);
  picture_ = new ImageGenerator(cfg);
  transformation_ = new DataComputer(cfg);
}

SpectrogramGenerator::~SpectrogramGenerator()
{
  if (data_)
    free(data_);
  delete decoder_;
  delete transformation_;
  delete picture_;
}

// decoder
void SpectrogramGenerator::openFile(std::string file_name)
{
  decoder_->openFile(file_name);
}

void SpectrogramGenerator::setupDecoder()
{
  decoder_->setup();
}

void SpectrogramGenerator::decodeAudioFile()
{
  decoder_->decode(&data_, &data_size_);
}

// data processing
void SpectrogramGenerator::processSamples()
{
  transformation_->processSamples(data_, data_size_);
}

// processed data visualization
void SpectrogramGenerator::generateSpectrograms()
{
  picture_->generateSpectrograms(transformation_->getPtrTransforms());
}