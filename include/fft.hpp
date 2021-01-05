#ifndef FFT_HPP
#define FFT_HPP

#include "ConfigReader.hpp"
#include "Decoder.hpp"

#include <vector>
#include <complex>

#include <fftw3.h>
#include <math.h>
#include <cstring>

class Fft_samples
{
public:
    Fft_samples(const GeneratorConfiguration *cfg);
    ~Fft_samples();

    void processSamples(const sample_fmt *data, uint data_size);
    void fillBuffer(float *outSamples, const sample_fmt *inPcmData, uint sampleCount);
    void hanningWindow();
    void runFft();

    std::vector<float *> specBuf;

private:
    const uint FFT_INPUT_SAMPLES;
    const uint FFT_OUTPUT_SAMPLES;

    float *inBuf, *windowedBuf;

    fftwf_complex *outBuf;
    uint inBufPos;

    fftwf_plan plan;
};

#endif