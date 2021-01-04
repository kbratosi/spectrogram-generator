#include "SpectrogramGenerator.hpp"
#include "fft.hpp"

#include <iostream>
#include <fstream>

#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

#include <boost/python.hpp>

namespace bp = boost::python;

int main()
{
  std::cout << "Hello world!" << std::endl;
  // Spooky scary skeleton!
  // *GUI
  
  // get input
  uint out_sample_rate,
       img_width,
       img_height,
       fft_input_size;
  float fft_overlapping;

  // initialize configuration script
  Py_Initialize();
  try {
    bp::object config_module = bp::import("config");
    bp::object config_namespace = config_module.attr("__dict__");
    //
    out_sample_rate = bp::extract<int  >(config_namespace["out_sample_rate"]);
    img_width       = bp::extract<int  >(config_namespace["img_width"      ]);
    img_height      = bp::extract<int  >(config_namespace["img_height"     ]);
    fft_input_size  = bp::extract<int  >(config_namespace["fft_input_size" ]);
    fft_overlapping = bp::extract<float>(config_namespace["fft_overlapping"]);
  }
  catch(bp::error_already_set const &) {
    PyErr_Print();
    return -1;
  }
  // 
  std::cout << "Read parameters:"
            << "\nout_sample_rate: " << out_sample_rate
            << "\nimg_width:       " << img_width
            << "\nimg_height:      " << img_height
            << "\nfft_input_size:  " << fft_input_size
            << "\nfft_overlapping: " << fft_overlapping 
            << std::endl;
  
  SpectrogramGenerator generator(out_sample_rate, fft_input_size);
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
}