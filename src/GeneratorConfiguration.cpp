#include "GeneratorConfiguration.hpp"

// read parameters from python configuration file
void GeneratorConfiguration::read() {
  // initialize configuration script
  Py_Initialize();
  try {
    bp::object config_module = bp::import("config");
    bp::object config_namespace = config_module.attr("__dict__");
    //
    out_sample_rate_         = bp::extract<uint >(config_namespace["out_sample_rate"        ]);
    img_width_               = bp::extract<uint >(config_namespace["img_width"              ]);
    img_height_              = bp::extract<uint >(config_namespace["img_height"             ]);
    img_scale_time_interval_ = bp::extract<uint >(config_namespace["img_scale_time_interval"]);
    img_fft_per_img_         = bp::extract<uint >(config_namespace["img_fft_per_img"        ]);
    fft_input_time_window_   = bp::extract<float>(config_namespace["fft_input_time_window"  ]);
    fft_overlap_             = bp::extract<float>(config_namespace["fft_overlap"            ]);
  }
  catch(bp::error_already_set const &) {
    throw;
  }
}

// calcutate complex parameters from other parameters
void GeneratorConfiguration::processParameters() {
  fft_in_frame_count_ = out_sample_rate_ * fft_input_time_window_ / 1000;
  fft_out_frame_count_ = fft_in_frame_count_ / 2 + 1;
  fft_delta_frame_ = (1 - fft_overlap_) * fft_in_frame_count_;
  img_time_per_img_ = (1 - fft_overlap_) * fft_input_time_window_ * img_fft_per_img_;
}


std::ostream& operator<<(std::ostream& os, const GeneratorConfiguration& cfg) {
  return os << "Generation parameters:"
            << "\nout_sample_rate:         " << cfg.out_sample_rate_
            << "\nimg_width:               " << cfg.img_width_
            << "\nimg_height:              " << cfg.img_height_
            << "\nimg_scale_time_interval: " << cfg.img_scale_time_interval_
            << "\nimg_fft_per_img:         " << cfg.img_fft_per_img_
            << "\nfft_input_time_window:   " << cfg.fft_input_time_window_
            << "\nfft_overlapping:         " << cfg.fft_overlap_
            << "\nCalculated parameters:"
            << "\nfft_in_frame_count:      " << cfg.fft_in_frame_count_
            << "\nfft_out_frame_count:     " << cfg.fft_out_frame_count_
            << "\ndelta_frame:             " << cfg.fft_delta_frame_
            << "\nimg_time_per_img:        " << cfg.img_time_per_img_
            << std::endl;
}