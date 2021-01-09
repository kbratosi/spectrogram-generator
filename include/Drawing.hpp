#ifndef SPECTROGRAM_HPP
#define SPECTROGRAM_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <omp.h>

class SpecImage
{
public:
    SpecImage(int height, int width, int inputSam, int numOfCol, float freqOnPx);
    ~SpecImage();
    bool saveImage(std::string value);
    int createImage(std::vector<float *> *data);
    void addBorder();
    void drawScale();

private:
    cv::Mat *image_, *tempImage_;
    int x_, y_;
    int imgResolution[2];
    float freqOnPx_;
};

#endif