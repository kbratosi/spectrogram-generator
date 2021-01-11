#include "SpectrogramGenerator.hpp"

SpecGen::SpectrogramGenerator(const GeneratorConfiguration *cfg)
{
  data_ = nullptr;
  data_size_ = 0;

  //float timePerImg = (1 - cfg->fft_overlap_) * cfg->img_fft_per_img_ / 1000 * cfg->fft_input_time_window_;

  decoder_ = new Decoder(cfg);
  picture_ = new ImageGenerator(cfg);
  transformation_ = new DataComputer(cfg);
}

SpecGen::~SpectrogramGenerator()
{
  if (data_)
    free(data_);
  delete decoder_;
  delete transformation_;
  delete picture_;
}

// decoder
void SpecGen::openFile(std::string file_name)
{
  decoder_->openFile(file_name);
}

void SpecGen::setupDecoder()
{
  decoder_->setup();
}

void SpecGen::decodeAudioFile()
{
  decoder_->decode(&data_, &data_size_);
}

// fft
void SpecGen::processSamples()
{
  transformation_->processSamples(data_, data_size_);
}

// visualization
void SpecGen::plotSpectrogram()
{
  picture_->createImage(transformation_->getSpecBuf());
}
