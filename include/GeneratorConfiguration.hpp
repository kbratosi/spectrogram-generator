/**
 * @file GeneratorConfiguration.hpp
 * @author Konrad Bratosiewicz
 * @brief File containing GeneratorConfiguration structure
 * @version 1.0
 * @date 2021-01-12
 */

#ifndef GENERATORCONFIGURATION_HPP
#define GENERATORCONFIGURATION_HPP

#include <iostream>
#include <boost/python.hpp>

namespace bp = boost::python;

/// Structure holding parameters required during spectrogram generation process. Responsible for:
/// * reading configuration file
/// * parameter validation
/// * doing necessary calculations on read parameters
/// * passing parameters to other modules
struct GeneratorConfiguration
{
      /// frequency of resampled audio file
  uint out_sample_rate_,
      /// parameter to declare number of FFTs on one image
      img_fft_per_img_,
      /// output resolution of spectrogram (without scale)
      img_width_,
      /// output resolution of spectrogram (without scale)
      img_height_,
      /// horizontal scale division unit
      img_scale_time_interval_;
      /// overlap (value from  0 to 1 )
  float fft_overlap_,
      /// size of window taken to FFT
      fft_input_time_window_;
      /// size of input data chunk
  float fft_in_frame_count_,
      /// offset between subsequent FFTs
      fft_delta_frame_;
      /// size of output transform
  uint fft_out_frame_count_,
      /// time of audio file displayed on one
      img_time_per_img_;

  /// read parameters from python configuration file
  void read();
  /// check whether selected parameters are correct
  void validateParameters();
  /// calcutate complex parameters from other parameters
  void processParameters();
};

/// display parameters stored in cfg struct
/// @see GeneratorConfiguration
std::ostream &operator<<(std::ostream &os, const GeneratorConfiguration &cfg);

#endif