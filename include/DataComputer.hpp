#ifndef DATACOMPUTER_HPP
#define DATACOMPUTER_HPP

#include "GeneratorConfiguration.hpp"
#include "Decoder.hpp"

#include <vector>
#include <complex>
#include <fftw3.h>
#include <math.h>
#include <cstring>
#include <omp.h>

/// Class used to process raw audio data into displayable format using FFT.
/// * FFT transforms discrete audio signal from time domain into frequency domain.
/// * Transformed signal is stored in DataComputer::transforms_
class DataComputer
{
public:
  ///@param cfg structure holding parameters required during spectrogram generation process
  ///@see GeneratorConfiguration
  DataComputer(const GeneratorConfiguration *cfg);
  ~DataComputer();

  ///Process discrete audio signal from time domain into frequency domain
  ///
  ///@param data input discrete audio signal
  ///@param data_size number of input samples
  void processSamples(const sample_fmt *data, uint data_size);

  /// apply Hanning window on computed data chunk in order to properly run FFT
  ///
  ///@param curr_window_head pointer to first element of processed data chunk
  void hanningWindow(const sample_fmt *curr_window_head);

  /// calculate FFT, write normalized transform output to transforms_
  void compute();

  std::vector<float *> *getTransforms();

private:
  const uint FFT_INPUT_SAMPLES;  //size of input data chunk
  const uint FFT_OUTPUT_SAMPLES; // size of output transform
  const float DELTA_FRAME;       //offset between subsequent FFTs

  fftwf_plan plan_;

  float *input_window_;
  fftwf_complex *output_buffer_;
  std::vector<float *> *transforms_; //vector to store arrays of scores of each FFT transformation
};

#endif