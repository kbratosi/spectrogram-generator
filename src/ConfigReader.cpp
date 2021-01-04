#include "ConfigReader.hpp"

void GeneratorConfiguration::readConfig() {
  // initialize configuration script
  Py_Initialize();
  try {
    bp::object config_module = bp::import("config");
    bp::object config_namespace = config_module.attr("__dict__");
    //
    out_sample_rate = bp::extract<uint >(config_namespace["out_sample_rate"]);
    img_width       = bp::extract<uint >(config_namespace["img_width"      ]);
    img_height      = bp::extract<uint >(config_namespace["img_height"     ]);
    fft_input_size  = bp::extract<uint >(config_namespace["fft_input_size" ]);
    fft_overlapping = bp::extract<float>(config_namespace["fft_overlapping"]);
  }
  catch(bp::error_already_set const &) {
    throw;
  }
}