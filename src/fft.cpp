#include "fft.hpp"
#include <cassert>

Fft_samples::Fft_samples(uint inputSamples) : FFT_INPUT_SAMPLES(inputSamples), FFT_OUTPUT_SAMPLES((inputSamples / 2) + 1)
{
    g_InBuf = new float[FFT_INPUT_SAMPLES];
    g_WindowedBuf = new float[FFT_INPUT_SAMPLES];
    g_InBufPos = FFT_INPUT_SAMPLES / 2;
    g_OutBuf = new fftwf_complex[FFT_OUTPUT_SAMPLES];
    g_Plan = fftwf_plan_dft_r2c_1d(FFT_INPUT_SAMPLES, g_WindowedBuf, g_OutBuf, FFTW_ESTIMATE);

    for (uint i = 0; i < FFT_INPUT_SAMPLES / 2; ++i)
        g_InBuf[i] = 0.f;
}

Fft_samples::~Fft_samples()
{
    free(g_InBuf);
    free(g_WindowedBuf);
    free(g_OutBuf);
    fftwf_destroy_plan(g_Plan);
}

void Fft_samples::processSamples(const void *inputBuf, uint bytes)
{
    const uint8_t *bufBytes = (const uint8_t *)inputBuf;
    uint samples = bytes / 2; //change this later on production | depends on size of one sample
    while (samples)
    {
        uint samplesToCopy = std::min(samples, FFT_INPUT_SAMPLES - g_InBufPos);
        normalizeSamples(g_InBuf + g_InBufPos, bufBytes, samplesToCopy);
        g_InBufPos += samplesToCopy;
        if (g_InBufPos == FFT_INPUT_SAMPLES)
        {
            runFft();
            memcpy(g_InBuf, g_InBuf + FFT_INPUT_SAMPLES / 2, sizeof(float) * FFT_INPUT_SAMPLES / 2);
            g_InBufPos = FFT_INPUT_SAMPLES / 2;
        }

        samples -= samplesToCopy;
        bufBytes += samplesToCopy * 4;
    }
}

void Fft_samples::normalizeSamples(float *outSamples, const uint8_t *inPcmData, uint sampleCount)
{
    for (uint i = 0; i < sampleCount; ++i, inPcmData += 2, ++outSamples)
    {
        short Sample = *(const short *)inPcmData;
        *outSamples = ((float)Sample) / 65536.f;
    }
}

void Fft_samples::hanningWindow()
{
    for (uint i = 0; i < FFT_INPUT_SAMPLES; ++i)
    {
        g_WindowedBuf[i] = g_InBuf[i];
        g_WindowedBuf[i] *= 0.54f - 0.46f * cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1));
    }
}

void Fft_samples::runFft()
{
    hanningWindow();
    fftwf_execute(g_Plan);

    for (uint i = 0; i < FFT_OUTPUT_SAMPLES; ++i)
    {
        g_OutBuf[i][0] = sqrt(pow(g_OutBuf[i][0], 2) + pow(g_OutBuf[i][1], 2));
    }
}