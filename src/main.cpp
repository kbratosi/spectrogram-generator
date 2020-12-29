#include "SpectrogramGenerator.hpp"

#include <iostream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

int main() {
  std::cout << "Hello world!" << std::endl;
  // Spooky scary skeleton!
  // *GUI
  // get input
  int sample_rate = 44100;
  // validate parameters

  // test for fftw3
  const int NUM_POINTS = 10;
  fftw_complex signal[NUM_POINTS];
  fftw_complex result[NUM_POINTS];

  fftw_plan plan = fftw_plan_dft_1d(NUM_POINTS,
                                    signal,
                                    result,
                                    FFTW_FORWARD,
                                    FFTW_ESTIMATE);

  SpectrogramGenerator generator(sample_rate);
  
  // generator.openFile();
  // generator.decodeFile();  // output: float/double

  // generator.transform(*start, size/ *end);
  // generator.createImage(*out_start);
}