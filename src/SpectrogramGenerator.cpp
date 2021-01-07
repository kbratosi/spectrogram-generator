#include "SpectrogramGenerator.hpp"

SpecGen::SpectrogramGenerator(const GeneratorConfiguration *cfg) {
  data_ = nullptr;
  data_size_ = 0;

  out_sample_rate_ = cfg->out_sample_rate;

  decoder_ = new Decoder(cfg);
  transformation_ = new Fft_samples(cfg);
  picture_ = new SpecImage(cfg->img_height,
                           cfg->img_width,
                           cfg->fft_in_frame_count,
                           cfg->fft_per_img           );
}


SpecGen::~SpectrogramGenerator()
{
  if (data_)
    free(data_);
  delete decoder_;
  delete transformation_;
  delete picture_;
}

void SpecGen::openFile(const char *file_name) {
  try {
    decoder_->openFile(file_name);
  }
  catch(std::exception &e) {
    throw e;
  }
}

void SpecGen::setupDecoder()
{
  try {
    decoder_->setup();
  }
  catch(std::exception &e) {
    throw std::runtime_error(std::string("Decoder setup: %s", e.what()));
  }
}

int SpecGen::decodeAudioFile()
{
  if (decoder_->readFile(&data_, &data_size_) != 0)
  {
    fprintf(stderr, "Failed to decode file");
    return -1;
  }
  return 0;
}

void SpecGen::processSamples()
{
  transformation_->processSamples(data_, data_size_);
}

void SpecGen::plotSpectrogram()
{
  picture_->createImage(transformation_->specBuf);
}