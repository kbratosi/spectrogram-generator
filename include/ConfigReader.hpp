#ifndef CONFIGREADER_HPP
#define CONFIGREADER_HPP

#include <boost/python.hpp>

namespace bp = boost::python;

struct GeneratorConfiguration {
  uint out_sample_rate,
       img_width,
       img_height,
       fft_input_size;
  float fft_overlapping;
  //
  void readConfig();
};


#endif