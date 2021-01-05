#include "ConfigReader.hpp"

void GeneratorConfiguration::read() {
  // initialize configuration script
  Py_Initialize();
  try {
    bp::object config_module = bp::import("config");
    bp::object config_namespace = config_module.attr("__dict__");
    //
    out_sample_rate       = bp::extract<uint >(config_namespace["out_sample_rate"      ]);
    img_width             = bp::extract<uint >(config_namespace["img_width"            ]);
    img_height            = bp::extract<uint >(config_namespace["img_height"           ]);
    fft_per_img           = bp::extract<uint >(config_namespace["fft_per_img"          ]);
    fft_input_time_window = bp::extract<float>(config_namespace["fft_input_time_window"]);
    fft_overlapping       = bp::extract<float>(config_namespace["fft_overlapping"      ]);
  }
  catch(bp::error_already_set const &) {
    throw;
  }
  fft_in_frame_count = out_sample_rate * fft_input_time_window / 1000;
  fft_out_frame_count = fft_in_frame_count / 2 + 1;
  delta_frame = (1 - fft_overlapping) * fft_in_frame_count;
}

std::ostream& operator<<(std::ostream& os, const GeneratorConfiguration& cfg) {
  return os << "Generation parameters:"
            << "\nout_sample_rate:       " << cfg.out_sample_rate
            << "\nimg_width:             " << cfg.img_width
            << "\nimg_height:            " << cfg.img_height
            << "\nfft_per_img:           " << cfg.fft_per_img
            << "\nfft_input_time_window: " << cfg.fft_input_time_window
            << "\nfft_overlapping:       " << cfg.fft_overlapping
            << "\nCalculated parameters:"
            << "\nfft_in_frame_count:    " << cfg.fft_in_frame_count
            << "\nfft_out_frame_count:   " << cfg.fft_out_frame_count
            << "\ndelta_frame:           " << cfg.delta_frame
            << std::endl;
}