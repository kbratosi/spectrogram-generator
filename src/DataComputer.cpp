/**
 * @file DataComputer.cpp
 * @author Jakub Marczuk
 * @brief File containing member functions of DataComputer
 * @version 1.0
 * @date 2021-01-12
 */

#include "DataComputer.hpp"

DataComputer::DataComputer(const GeneratorConfiguration *cfg) : FFT_INPUT_SAMPLES(cfg->fft_in_frame_count_),
                                                                FFT_OUTPUT_SAMPLES(cfg->fft_out_frame_count_),
                                                                DELTA_FRAME(cfg->fft_delta_frame_)
{
  fftw_init_threads();
  fftw_plan_with_nthreads(omp_get_max_threads());

  //init arrays & plan to run FFT
  input_window_ = new float[FFT_INPUT_SAMPLES];
  output_buffer_ = new fftwf_complex[FFT_OUTPUT_SAMPLES];
  plan_ = fftwf_plan_dft_r2c_1d(FFT_INPUT_SAMPLES, input_window_, output_buffer_, FFTW_ESTIMATE);
}

DataComputer::~DataComputer()
{
  delete[] input_window_;
  delete[] output_buffer_;
  for (uint i = 0; i < transforms_.size(); ++i)
  {
    delete[] transforms_.at(i);
  }
  fftwf_destroy_plan(plan_);
  fftw_cleanup_threads();
}

void DataComputer::processSamples(const SampleFormat *data, uint data_size)
{
  float curr_frame = 0;
  while (curr_frame + FFT_INPUT_SAMPLES <= static_cast<float>(data_size))
  {
    hanningWindow(data + static_cast<int>(curr_frame));
    compute();
    curr_frame += DELTA_FRAME;
  }
}

void DataComputer::hanningWindow(const SampleFormat *curr_window_head)
{
  for (uint i = 0; i < FFT_INPUT_SAMPLES; ++i)
  {
    input_window_[i] = *(curr_window_head + i);
    input_window_[i] *= 0.5f * (1 - cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1)));
  }
}

void DataComputer::compute()
{
  fftwf_execute(plan_);

  float *temp_buf = new float[FFT_OUTPUT_SAMPLES];
  // normalize output logarithmically
  for (uint i = 0; i < FFT_OUTPUT_SAMPLES; ++i)
  {
    output_buffer_[i][0] *= (2. / FFT_INPUT_SAMPLES);
    output_buffer_[i][1] *= (2. / FFT_INPUT_SAMPLES);
    temp_buf[i] = 10. / log(10.) * log(pow(output_buffer_[i][0], 2) + pow(output_buffer_[i][1], 2) + 1e-6); //max value - 60dB
  }
  transforms_.push_back(temp_buf);
}

std::vector<float *> *DataComputer::getPtrTransforms()
{
  return &transforms_;
}