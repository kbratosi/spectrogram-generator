#include "SpectrogramGenerator.hpp"

SpecGen::SpectrogramGenerator(const int sample_rate)
{
  data_ = nullptr;
  data_size_ = 0;
  sample_rate_ = sample_rate;
}

SpecGen::~SpectrogramGenerator()
{
  if (data_)
    free(data_);
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