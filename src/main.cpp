#include "SpectrogramGenerator.hpp"

#include <iostream>
#include <fstream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

int main() {
  std::cout << "Hello world!" << std::endl;
  // Spooky scary skeleton!
  // *GUI
  // get input
  int out_sample_rate = 44100;
  // validate parameters

  SpectrogramGenerator generator(out_sample_rate);
  std::cout << "Decoder" << std::endl;
  if(generator.setupDecoder("audio/440hz.mp3") != 0)
    return -1;
  if(generator.decodeAudioFile() != 0)
    return -1;
  
  // temporary - delete later!
  std::ofstream decoded;
  decoded.open("test");
  decoded.write((char *)generator.data_, generator.data_size_);
  decoded.close();

  // generator.transform(*start, size/ *end);
  // generator.createImage(*out_start);
}