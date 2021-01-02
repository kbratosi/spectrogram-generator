#include "fft.hpp"
#include <cassert>

Fft_samples::Fft_samples(uint inputSamples) : FFT_INPUT_SAMPLES(inputSamples), FFT_OUTPUT_SAMPLES((inputSamples / 2) + 1)
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
    free(inBuf);
    free(windowedBuf);
    free(outBuf);
    fftwf_destroy_plan(plan);
}

void Fft_samples::processSamples(const void *inputBuf, uint bytes)
{
    const sample_fmt *bufBytes = (const sample_fmt *)inputBuf;
    uint samples = bytes; //change this later on production | depends on size of one sample
    while (samples)
    {
        uint samplesToCopy = std::min(samples, FFT_INPUT_SAMPLES - inBufPos);
        normalizeSamples(inBuf + inBufPos, bufBytes, samplesToCopy);
        inBufPos += samplesToCopy;
        if (inBufPos == FFT_INPUT_SAMPLES)
        {
            runFft();
            memcpy(inBuf, inBuf + FFT_INPUT_SAMPLES / 2, sizeof(float) * FFT_INPUT_SAMPLES / 2);
            inBufPos = FFT_INPUT_SAMPLES / 2;
        }

        samples -= samplesToCopy;
        bufBytes += samplesToCopy * sizeof(sample_fmt);
    }
}

void Fft_samples::normalizeSamples(float *outSamples, const sample_fmt *inPcmData, uint sampleCount)
{
    for (uint i = 0; i < sampleCount; ++i, inPcmData += sizeof(sample_fmt), ++outSamples)
    {
        short Sample = *(const short *)inPcmData; //ask yourself is it obligatory??? does it make sense?
        *outSamples = ((float)Sample) / 65536.f;
    }
}

void Fft_samples::hanningWindow()
{
    for (uint i = 0; i < FFT_INPUT_SAMPLES; ++i)
    {
        windowedBuf[i] = inBuf[i];
        windowedBuf[i] *= 0.54f - 0.46f * cosf((M_PI * 2.f * i) / (FFT_INPUT_SAMPLES - 1));
    }
}

void Fft_samples::runFft()
{
    hanningWindow();
    fftwf_execute(plan);

    for (uint i = 0; i < FFT_OUTPUT_SAMPLES; ++i)
    {
        outBuf[i][0] = sqrt(pow(outBuf[i][0], 2) + pow(outBuf[i][1], 2));
    }

    fftwf_complex *tempBuf = new fftwf_complex[FFT_OUTPUT_SAMPLES];
    std::memcpy(tempBuf, outBuf, sizeof(fftwf_complex) * FFT_OUTPUT_SAMPLES);
    specBuf.push_back(tempBuf);
}