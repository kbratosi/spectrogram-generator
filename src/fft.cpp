#include "fft.hpp"
#include <cassert>
#include <iostream>

Fft_samples::Fft_samples(const GeneratorConfiguration *cfg) : FFT_INPUT_SAMPLES(cfg->fft_in_frame_count),
                                                              FFT_OUTPUT_SAMPLES(cfg->fft_out_frame_count),
                                                              DELTA_FRAME(cfg->delta_frame)
{
  fftw_init_threads();
  fftw_plan_with_nthreads(omp_get_max_threads());

  input_window_ = new float[FFT_INPUT_SAMPLES];
  output_buffer_ = new fftwf_complex[FFT_OUTPUT_SAMPLES];
  plan = fftwf_plan_dft_r2c_1d(FFT_INPUT_SAMPLES, input_window_, output_buffer_, FFTW_ESTIMATE);
}

Fft_samples::~Fft_samples()
{
  delete[] input_window_;
  delete[] output_buffer_;
  for (int i = 0; i < specBuf.size(); ++i)
  {
    delete[] specBuf[i];
  }
  fftwf_destroy_plan(plan);
  fftw_cleanup_threads();
  //fftw_cleanup();
}

void Fft_samples::processSamples(const sample_fmt *data, uint data_size)
{
  float curr_frame = 0;
  while (curr_frame + FFT_INPUT_SAMPLES <= static_cast<float>(data_size))
  {
    hanningWindow(data + static_cast<int>(curr_frame));
    runFft();
    curr_frame += DELTA_FRAME;
  }
}

void Fft_samples::hanningWindow(const sample_fmt *curr_window_head)
{
  for (uint i = 0; i < FFT_INPUT_SAMPLES; ++i)
  {
    input_window_[i] = *(curr_window_head + i);
    // windowedBuf[i] *= 0.54f - 0.46f * cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1));
    input_window_[i] *= 0.5f * (1 - cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1)));
  }
}

void Fft_samples::runFft()
{
  fftwf_execute(plan);

  float *tempBuf = new float[FFT_OUTPUT_SAMPLES];

  for (uint i = 0; i < FFT_OUTPUT_SAMPLES; ++i)
  {
    output_buffer_[i][0] *= (2. / FFT_INPUT_SAMPLES);
    output_buffer_[i][1] *= (2. / FFT_INPUT_SAMPLES);
    tempBuf[i] = 10. / log(10.) * log(pow(output_buffer_[i][0], 2) + pow(output_buffer_[i][1], 2) + 1e-6); //max value - 96dB
  }

  specBuf.push_back(tempBuf);
}