#ifndef IMAGEGENERATOR_HPP
#define IMAGEGENERATOR_HPP

#include "GeneratorConfiguration.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <omp.h>

///Create set of spectrograms
class ImageGenerator
{
public:
    ///@param cfg structure holding parameters required during spectrogram generation process
    ///@see GeneratorConfiguration
    ImageGenerator(const GeneratorConfiguration *cfg);
    ~ImageGenerator();
    ///save image into declared by user spectrogram's resolution
    ///@param value information about order of created spectrograms
    bool saveImage(std::string value);
    ///create set of images based on data from FFT
    ///@param data vector which stores processed data in frequency domain
    void createImage(std::vector<float *> *data);
    ///draw scale on image
    ///@param value information about presently created spectrogram to scale properly time scale line
    void drawScale(std::string value);
    ///draw scale lines on image
    ///@param point0 array which stores intersection point of scale lines
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
    int x_, y_, scale_time_, sample_rate_, time_per_img_, scale_time_interval_;
};

#endif