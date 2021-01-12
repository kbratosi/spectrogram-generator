@page subsystem1 Spectrogram Generator
@tableofcontents

Application to plot spectrograms from audio files.

@section subsystem1section1 Dependencies

* Requirements

    `sudo apt install cmake pkg-config ffmpeg libopencv-dev libfftw3-dev libomp-dev python`

* Optional (for tests and creating documentation)

    `sudo apt install libboost-all-dev doxygen`

@section subsystem1section2 Building and Running
* Download repository

    `git clone https://gitlab.com/ProtegerPW/spectograms.git`

* To build application

    `./build.sh`
  
* Run application
  
    `./run.sh <path to audio file>`

* Run unit tests
    `./build.sh -DBUILD_TESTS=TRUE`

    `./build/unit_tests`

* Build documentation
    `./build.sh -DBUILD_DOCS=TRUE`

    index.html is located in docs/html

@section subsystem1section3 Documentation

    [documentation](https://protegerpw.gitlab.io/spectograms/index.html)


