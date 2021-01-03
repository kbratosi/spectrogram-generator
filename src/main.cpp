#include "SpectrogramGenerator.hpp"
#include "fft.hpp"

#include <iostream>
#include <fstream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/python.hpp>

extern "C" PyObject *PyInit_config();

int main()
{
  std::cout << "Hello world!" << std::endl;
  int out_sample_rate = -1;
  // Spooky scary skeleton!
  // *GUI
  // get input

  // Initialize Python
  setenv("PYTHONPATH", ".", 1);
  Py_Initialize();
  boost::python::object config_module = boost::python::import("config");
  boost::python::object config_namespace = config_module.attr("__dict__");
  try {
    out_sample_rate = boost::python::extract<int>(config_namespace["out_sample_rate"]);
  }
  catch(boost::python::error_already_set const &) {
    PyErr_Print();
  }
  //
  std::cout << out_sample_rate << std::endl;
  int inputSamples = 4096;
  // validate parameters
  
  /* uncomment later!
  SpectrogramGenerator generator(out_sample_rate, inputSamples);
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

  generator.processSamples();

  std::ofstream firer;
  firer.open("wyniki.txt");

  for (int i = 0; i < generator.transformation_->specBuf.size(); ++i)
  {
    //firer << std::endl << "new frame " << i << std::endl;
    for (int j = 0; j < 256 / 2 + 1; ++j)
    {
      firer << (generator.transformation_->specBuf[i][j]) << std::endl;
      // }
    }
  }
  firer.close();

  // generator.transform(*start, size/ *end);
  // generator.createImage(*out_start);
  */
}