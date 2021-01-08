#ifndef SPECTROGRAM_HPP
#define SPECTROGRAM_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <omp.h>

class SpecImage
{
public:
    SpecImage(int height, int width, int inputSam, int numOfCol);
    ~SpecImage();
    bool saveImage(std::string value);
    int createImage(std::vector<float *> *data);

private:
    cv::Mat *image_, *tempImage_;
    int x_, y_;
};

#endif