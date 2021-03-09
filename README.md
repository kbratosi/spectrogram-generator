# Spectrogram generator

Spectrogram generator is a CLI application decoding, computing and plotting a set of spectrograms from an audio file. Decoding is accomplished using FFMpeg, computation of Fast Fourier Transform using FFTW and plotting of spectrogram bitmaps using OpenCV.

## Features
* program supports many popular audio formats (courtesy of FFMpeg)
* run-time parameters parsing from a Python script (config.py)
* user can adjust following parameters:
    - FFT window size, overlap
    - number of FFTs per image
    - image: height, width \[px\]
    - horizontal scale division unit \[ms\]
* spectrograms are saved in .png format (in output/)

## Output example 
![](/docs/Spectrogram0.png)

## Requirements

    apt install cmake pkg-config ffmpeg libopencv-dev libfftw3-dev libomp-dev python (requires root)

### Optional (for tests and creating documentation)

    apt install libboost-all-dev doxygen (requires root)

## Download repository

    git clone https://gitlab.com/ProtegerPW/spectrograms.git

## Build application

    cd spectrograms
    ./build.sh
  
## Run application
  
    ./run.sh <path to_audio_file>

## Run unit tests

    ./build.sh -DBUILD_TESTS=TRUE

    ./build/unit_tests

## Build documentation

    ./docs.sh

file index.html is located in docs/html

## Documentation

[documentation](https://protegerpw.gitlab.io/spectrograms/index.html)
