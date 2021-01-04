#include "SpectrogramGenerator.hpp"

SpecGen::SpectrogramGenerator(const int sample_rate, const uint inputSamples, int height, int width, int numOfCol)
{
  data_ = nullptr;
  data_size_ = 0;
  sample_rate_ = sample_rate;
  transformation_ = new Fft_samples(inputSamples);
  picture_ = new SpecImage(height, width, inputSamples, numOfCol);
}

SpecGen::~SpectrogramGenerator()
{
  if (data_)
    free(data_);
  delete transformation_;
}

int SpecGen::setupDecoder(const char *file_name)
{
  if (decoder_.setup(file_name, sample_rate_) != 0)
  {
    fprintf(stderr, "Failed to setup decoder\n");
    return -1;
  }
  return 0;
}

int SpecGen::decodeAudioFile()
{
  if (decoder_.readFile(&data_, &data_size_) != 0)
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