#ifndef FFT_HPP
#define FFT_HPP

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
    void normalizeSamples(float *outSamples, const uint8_t *inPcmData, uint sampleCount);
    void hanningWindow();
    void runFft();

private:
    const uint FFT_INPUT_SAMPLES;
    const uint FFT_OUTPUT_SAMPLES;

    float *g_InBuf, *g_WindowedBuf;
    uint g_InBufPos;
    fftwf_complex *g_OutBuf;
    fftwf_plan g_Plan;
};

#endif