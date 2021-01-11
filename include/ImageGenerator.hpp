#ifndef SPECTROGRAM_HPP
#define SPECTROGRAM_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <omp.h>

class ImageGenerator
{
public:
    ImageGenerator(int height, int width, int inputSam, int numOfCol, int samplingRate, int timePerImg, int timeInterval);
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
    timePerImg_ - calculated time displayed on one image
    */
    int x_, y_, scaleTime, samplingRate_, timePerImg_, timeInterval_;
};

#endif