# Spectrogram generator

Application to plot set of spectrograms from audio files

![](/docs/Spectrogram0.png)

## Features
* spectrograms are saved in .png format (in output folder)
* program supports many popular audio formats
* user can adjust attributes of image (height, width, overlapping, FFT on one image)


## Requirements

    sudo apt install cmake pkg-config ffmpeg libopencv-dev libfftw3-dev libomp-dev python

### Optional (for tests and creating documentation)

    sudo apt install libboost-all-dev doxygen

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
