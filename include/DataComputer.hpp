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

/* Class used to process raw audio data into displayable format using FFT.
 * FFT transforms discrete audio signal from time domain into frequency domain.
 * Transformed signal is stored in <NAME!!!>                                    */
class DataComputer
{
public:
  DataComputer(const GeneratorConfiguration *cfg);
  ~DataComputer();

  void processSamples(const sample_fmt *data, uint data_size);
  // apply Hanning window on computed data chunk in order to properly run FFT
  void hanningWindow(const sample_fmt *curr_window_head);
  // calculate FFT, write normalized transform to specBuf
  void compute();

  std::vector<float *> *getSpecBuf();

private:
  const uint FFT_INPUT_SAMPLES;
  const uint FFT_OUTPUT_SAMPLES;
  const float DELTA_FRAME;

  fftwf_plan plan_;

  float *input_window_;
  fftwf_complex *output_buffer_;
  std::vector<float *> *specBuf; //vector to store arrays of scores of each FFT transformation
};

#endif