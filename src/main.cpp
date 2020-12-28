#include "SpectrogramGenerator.hpp"

#include <iostream>

#include <libavcodec/avcodec.h>
#include <fftw3.h>
#include <opencv2/opencv.hpp>
#include <boost/test/unit_test.hpp>

int main() {
    std::cout << "Hello world!" << std::endl;
    // Spooky scary skeleton!
    // *GUI
    // get input
    // validate parameters
    SpectrogramGenerator generator;
    
    // generator.openFile();
    // generator.decodeFile();  // output: float/double

    // generator.transform(*start, size/ *end);
    // generator.createImage(*out_start);
}