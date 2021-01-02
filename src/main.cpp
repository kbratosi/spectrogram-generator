#include "SpectrogramGenerator.hpp"
#include "fft.hpp"

#include <iostream>
#include <fstream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

int main()
{
  std::cout << "Hello world!" << std::endl;
  // Spooky scary skeleton!
  // *GUI
  // get input
  int out_sample_rate = 44100;
  // validate parameters

  SpectrogramGenerator generator(out_sample_rate);
  std::cout << "Decoder" << std::endl;
  if (generator.setupDecoder("audio/440hzShort.mp3") != 0)
    return -1;
  if (generator.decodeAudioFile() != 0)
    return -1;

  // temporary - delete later!
  std::ofstream decoded;
  decoded.open("test");
  decoded.write((char *)generator.data_, generator.data_size_ * sizeof(sample_fmt));
  decoded.close();

  std::cout << generator.data_size_ << " " << sizeof(fftw_complex) << std::endl;

  Fft_samples transformation(4096);
  transformation.processSamples(generator.data_, generator.data_size_);

  std::ofstream firer;
  firer.open("wyniki.txt");

  // for (int i = 0; i < transformation.specBuf.size(); ++i)
  // {
  //   //firer << std::endl << "new frame " << i << std::endl;
  //   for (int j = 0; j < 512 / 2 + 1; ++j)
  //   {
  //     firer << *(transformation.specBuf[i] + j)[0] << std::endl;
  //   // }
  // }
  for (int j = 0; j < 4096 / 2 + 1; ++j)
  {
    firer << *(transformation.specBuf[64] + j)[0] << std::endl;
  }
  firer.close();

  // generator.transform(*start, size/ *end);
  // generator.createImage(*out_start);
}