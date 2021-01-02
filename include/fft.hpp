#ifndef FFT_HPP
#define FFT_HPP

#include "Decoder.hpp"

#include <vector>
#include <complex>

#include <fftw3.h>
#include <math.h>
#include <cstring>

class Fft_samples
{
public:
    Fft_samples(uint inputSamples);
    ~Fft_samples();

    void processSamples(const void *inputBuf, uint bytes);
    void normalizeSamples(float *outSamples, const sample_fmt *inPcmData, uint sampleCount);
    void hanningWindow();
    void runFft();

    fftwf_complex *outBuf;

private:
    const uint FFT_INPUT_SAMPLES;
    const uint FFT_OUTPUT_SAMPLES;

    float *inBuf, *windowedBuf;
    uint inBufPos;

    std::vector<fftwf_complex *> specBuf;
    fftwf_plan plan;
};

#endif