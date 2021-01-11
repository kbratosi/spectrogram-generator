#include "SpectrogramGenerator.hpp"
#include "FFT.hpp"
#include "Drawing.hpp"
#include "GeneratorConfiguration.hpp"
#include "Benchmark.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
  if (argv[1] == nullptr || argc > 2)
  {
    std::cerr << "Wrong number of parameters" << std::endl;
    std::cout << "Usage: ./run.sh <path_to_file>" << std::endl;
    return -1;
  }
  std::string file_name = argv[1];

  GeneratorConfiguration config;
  try
  {
    config.read();
    config.processParameters();
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
  try
  {
    generator.openFile(file_name);
    generator.setupDecoder();
    generator.decodeAudioFile();
  }
  catch (std::exception &e)
  {
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
