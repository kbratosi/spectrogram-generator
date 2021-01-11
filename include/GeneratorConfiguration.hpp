#ifndef CONFIGREADER_HPP
#define CONFIGREADER_HPP

#include <iostream>
#include <boost/python.hpp>

namespace bp = boost::python;

/* Structure holding parameters required during spectrogram generation process.
 * Responsible for reading configuration file, doing necessary calculations on read parameters
 * and passing parameters to modules.                                                          */
struct GeneratorConfiguration {
  uint out_sample_rate_,
       img_fft_per_img_,
       img_width_,
       img_height_,
       img_scale_time_interval_;
  float fft_overlap_,
        fft_input_time_window_;
  //
  float fft_in_frame_count_,
        fft_delta_frame_;
  uint fft_out_frame_count_,
       img_time_per_img_;

  // read parameters from python configuration file
  void read();
  // calcutate complex parameters from other parameters
  void processParameters();
};

std::ostream& operator<<(std::ostream& os, const GeneratorConfiguration& cfg);

#endif