#include "SpectrogramGenerator.hpp"
#include "fft.hpp"
#include "drawImg.hpp"
#include "ConfigReader.hpp"
#include "Benchmark.hpp"

#include <iostream>
#include <fstream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/python.hpp>

int main()
{
  std::cout << "Hello world!" << std::endl;
  // Spooky scary skeleton!
  // *GUI

  // get input
  GeneratorConfiguration config;
  try
  {
    config.read();
  }
  catch (boost::python::error_already_set const &)
  {
    PyErr_Print();
    return -1;
  }
  // display read parameters
  std::cout << config;

  // validate parameters!

  SpectrogramGenerator generator(&config);

  std::cout << "Decoder" << std::endl;
  try {
    generator.openFile("audio/yomama.mp3");
    generator.setupDecoder();
    generator.decodeAudioFile();
  }
  catch(std::exception &e) {
    std::cerr << e.what() << std::endl
              << "Shutting down..." << std::endl;
    return -1;
  }

  std::cout << "FFT" << std::endl;
  Benchmark<std::chrono::milliseconds> fftTime;
  generator.processSamples();
  size_t score1 = fftTime.elapsed();

  std::cout << "OpenCV" << std::endl;
  Benchmark<std::chrono::milliseconds> cvTime;
  generator.plotSpectrogram();
  size_t score2 = cvTime.elapsed();

  std::cout << "FFT: " << score1 << " ms" << std::endl
            << "OpenCV: " << score2 << " ms" << std::endl;
}