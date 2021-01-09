#include "SpectrogramGenerator.hpp"

SpecGen::SpectrogramGenerator(const GeneratorConfiguration *cfg)
{
  data_ = nullptr;
  data_size_ = 0;

  out_sample_rate_ = cfg->out_sample_rate;

  float timePerImg = (1 - cfg->fft_overlapping) * cfg->fft_per_img / 1000 * cfg->fft_input_time_window;

  decoder_ = new Decoder(cfg);
  transformation_ = new Fft_samples(cfg);
  picture_ = new SpecImage(cfg->img_height,
                           cfg->img_width,
                           cfg->fft_in_frame_count,
                           cfg->fft_per_img,
                           cfg->out_sample_rate,
                           timePerImg);
}

SpecGen::~SpectrogramGenerator()
{
  if (data_)
    free(data_);
  delete decoder_;
  delete transformation_;
  delete picture_;
}

int SpecGen::setupDecoder(const char *file_name)
{
  if (decoder_->setup(file_name, out_sample_rate_) != 0)
  {
    fprintf(stderr, "Failed to setup decoder\n");
    return -1;
  }
  return 0;
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
  picture_->createImage(transformation_->getSpecBuf());
}