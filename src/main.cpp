#include "SpectrogramGenerator.hpp"

#include <iostream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>

int main() {
  std::cout << "Hello world!" << std::endl;
  // Spooky scary skeleton!
  // *GUI
  // get input
  int sample_rate = 44100;
  // validate parameters

  SpectrogramGenerator generator(sample_rate);
  
  // generator.openFile();
  generator.decodeAudioFile("audio/440hz.mp3");  // output: float/double
  std::ofstream decoded;
  decoded.open("test");
  decoded.write((char *)generator.data_, generator.data_size_ * sizeof(sample_fmt));
  decoded.close();

  std::cout << generator.data_size_ << std::endl;
  // generator.transform(*start, size/ *end);
  // generator.createImage(*out_start);
}