#ifndef IMAGEGENERATOR_HPP
#define IMAGEGENERATOR_HPP

#include "ConfigReader.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <omp.h>

class ImageGenerator
{
public:
    ImageGenerator(const GeneratorConfiguration *cfg;
    ~ImageGenerator();

    bool saveImage(std::string value);
    void createImage(std::vector<float *> *data);
    void drawScale(std::string value);
    void addScaleLines(int point0[]);

private:
    cv::Mat *image_, *outputImage_, *tempImage_;
    /*
    x_ - width of image create from FFT transformation
    y_ - height --- || --- || --- || --- 
    scaleTime - variable used to help displaying value on scale
    timePerImg_ - calculated time in ms displayed on one image
    timeI
    */
    int x_, y_, scaleTime, samplingRate_, timePerImg_, scale_time_interval_;
};

#endif