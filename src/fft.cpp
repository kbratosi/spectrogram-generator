#include "fft.hpp"
#include <cassert>
#include <iostream>

Fft_samples::Fft_samples(const GeneratorConfiguration *cfg):
  FFT_INPUT_SAMPLES(cfg->fft_in_frame_count),
  FFT_OUTPUT_SAMPLES(cfg->fft_out_frame_count)
{
  inBuf = new float[FFT_INPUT_SAMPLES];
  windowedBuf = new float[FFT_INPUT_SAMPLES];
  inBufPos = FFT_INPUT_SAMPLES / 2;
  outBuf = new fftwf_complex[FFT_OUTPUT_SAMPLES];
  plan = fftwf_plan_dft_r2c_1d(FFT_INPUT_SAMPLES, windowedBuf, outBuf, FFTW_ESTIMATE);

  for (uint i = 0; i < FFT_INPUT_SAMPLES / 2; ++i)
    inBuf[i] = 0.f;
}

Fft_samples::~Fft_samples()
{
    delete[] inBuf;
    delete[] windowedBuf;
    delete[] outBuf;
    for (int i = 0; i < specBuf.size(); ++i)
    {
        delete[] specBuf[i];
    }
    fftwf_destroy_plan(plan);
    fftw_cleanup();
}

void Fft_samples::processSamples(const sample_fmt *data, uint data_size)
{
    const sample_fmt *bufBytes = data;
    uint samples = data_size;
    int iter = 0;
    while (samples)
    {
        uint samplesToCopy = std::min(samples, FFT_INPUT_SAMPLES - inBufPos);
        std::cout << "Samples to copy: " << samplesToCopy << " iteration: " << ++iter << std::endl;
        fillBuffer(inBuf + inBufPos, bufBytes, samplesToCopy);
        inBufPos += samplesToCopy;
        if (inBufPos == FFT_INPUT_SAMPLES) //fill rest of samples with 0 to calculate FFT
        {
            runFft();
            memcpy(inBuf, inBuf + FFT_INPUT_SAMPLES / 2, sizeof(float) * FFT_INPUT_SAMPLES / 2);
            inBufPos = FFT_INPUT_SAMPLES / 2;
        }

        samples -= samplesToCopy;
        bufBytes += samplesToCopy;
    }
}

void Fft_samples::fillBuffer(float *outSamples, const sample_fmt *inPcmData, uint sampleCount)
{
    // for (uint i = 0; i < sampleCount; ++i, ++inPcmData, ++outSamples)
    // {
    //short Sample = *(const short *)inPcmData; //ask yourself is it obligatory??? does it make sense?
    memcpy(outSamples, inPcmData, sizeof(sample_fmt) * sampleCount);
    // }
}

void Fft_samples::hanningWindow()
{
    for (uint i = 0; i < FFT_INPUT_SAMPLES; ++i)
    {
        windowedBuf[i] = inBuf[i];
        // windowedBuf[i] *= 0.54f - 0.46f * cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1));
        windowedBuf[i] *= 0.5f * (1 - cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1)));
    }
}

void Fft_samples::runFft()
{
    hanningWindow();
    fftwf_execute(plan);

    float *tempBuf = new float[FFT_OUTPUT_SAMPLES];

    for (uint i = 0; i < FFT_OUTPUT_SAMPLES; ++i)
    {
        outBuf[i][0] *= (2. / FFT_INPUT_SAMPLES);
        outBuf[i][1] *= (2. / FFT_INPUT_SAMPLES);
        tempBuf[i] = 10. / log(10.) * log(pow(outBuf[i][0], 2) + pow(outBuf[i][1], 2) + 1e-6); //max value - 96dB
    }

    specBuf.push_back(tempBuf);
}