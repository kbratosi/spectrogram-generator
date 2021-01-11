#include "Drawing.hpp"

SpecImage::SpecImage(int height, int width, int inputSam, int numOfCol, int samplingRate, float timePerImg)
{
    y_ = inputSam / 2 + 1; //transform to number of output samples
    x_ = numOfCol;         // num of columns on image
    samplingRate_ = samplingRate / 2;
    timePerImg_ = timePerImg;
    scaleTime = 0;

    //output image
    image_ = new cv::Mat(height, width, CV_16UC1, cv::Scalar(0));
    outputImage_ = new cv::Mat();
    //create temporary image based on original data and then scale it
    tempImage_ = new cv::Mat(y_, x_, CV_16UC1, cv::Scalar(0));

    if (image_->empty() || tempImage_->empty())
    {
        throw std::runtime_error("Error: couldn`t create image");
    }
}

SpecImage::~SpecImage()
{
    delete image_;
    delete tempImage_;
    delete outputImage_;
}

void SpecImage::createImage(std::vector<float *> *data)
{
    //create images from full windows
    int numOfImages = data->size() / x_;
    for (int k = 0; k < numOfImages; ++k)
    {
#pragma omp parallel for
        for (int i = 0; i < x_; ++i)
        {
            for (int j = 0; j < y_; ++j)
            {
                cv::Point p(i, j);
                cv::Scalar colorIn((data->at(x_ * k + i)[j] / 60 + 1) * 65536); //normalize output from 0 to 65536
                cv::line(*tempImage_, p, p, colorIn, 2);
            }
        }
        if (!saveImage(std::to_string(k)))
            throw std::runtime_error("Error: couldn't save image");
    }

    //create image from remaining samples
    *tempImage_ = cv::Scalar(0); //clear tempImage
#pragma omp parallel for
    for (uint i = 0; i < (data->size() - (x_ * numOfImages)); ++i)
    {
        for (int j = 0; j < y_; ++j)
        {
            cv::Point p(i, j);
            cv::Scalar colorIn((data->at(x_ * numOfImages + i)[j] / 60 + 1) * 65536); //normalize output from 0 to 65536
            cv::line(*tempImage_, p, p, colorIn, 2);
        }
    }
    if (!saveImage(std::to_string(numOfImages)))
        throw std::runtime_error("Error: couldn't save image");
}

bool SpecImage::saveImage(std::string value)
{
    //scale tempImage to resolution of output image dedclared by user
    cv::resize(*tempImage_, *image_, image_->size());
    drawScale(value);
    return cv::imwrite("./output/Spectrogram" + value + ".png", *outputImage_);
}

void SpecImage::addScaleLines(int point0[])
{
    //init width and height of borders + color

    //TODO co ze skalą? robimy dynamiczną?
    //int top = (int)(0.02 * image_->rows);
    //int left = (int)(0.015 * image_->cols);
    //int top = 18, right = 35;
    cv::Scalar background(0);

    cv::copyMakeBorder(*image_, *outputImage_, point0[1], 0, 0, point0[0], cv::BORDER_CONSTANT, background);

    cv::Scalar colorIn(65000);

    cv::Point beginH(0, point0[1] - 1);
    cv::Point endH(outputImage_->cols, point0[1] - 1);
    cv::line(*outputImage_, beginH, endH, colorIn, 1);

    cv::Point beginV(outputImage_->cols - point0[0], 0);
    cv::Point endV(outputImage_->cols - point0[0], outputImage_->rows);
    cv::line(*outputImage_, beginV, endV, colorIn, 1);
}

void SpecImage::drawScale(std::string value)
{
    //set point to cut scale Lines (upper - right corner)
    int point0[2] = {45, 18};
    addScaleLines(point0);

    cv::Scalar colorIn(65000);
    // OY
    cv::putText(*outputImage_, "Hz", cv::Point(outputImage_->cols - point0[0] + 6, point0[1] + 10), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1.5);
    //set to draw scale every 1 kHz
    int stepY = (1000 * outputImage_->rows) / samplingRate_;

#pragma omp parallel for
    for (int i = point0[1] + stepY; i < outputImage_->rows; i += stepY)
    {
        cv::line(*outputImage_, cv::Point(outputImage_->cols - point0[0], i), cv::Point(outputImage_->cols - point0[0] + 4, i), colorIn, 1);

        cv::putText(*outputImage_, std::to_string(((i - point0[1]) / stepY) * 1000), cv::Point(outputImage_->cols - point0[0] + 6, i), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1);
    }

    //OX
    //set to draw scale every 5 seconds - calculate step
    int stepX = (5 * outputImage_->cols) / timePerImg_;
    //calculate how many values program can draw
    int howManyValues = outputImage_->cols / stepX;
    //calculate how many FFT remain
    int remainingFft = outputImage_->cols - stepX * howManyValues;
    int numOfImg = std::stoi(value);

    for (int i = ((numOfImg * remainingFft) % stepX) + point0[0]; i < outputImage_->cols; i += stepX, ++scaleTime)
    {

        cv::line(*outputImage_, cv::Point(i, 12), cv::Point(i, 17), colorIn, 1);

        cv::putText(*outputImage_, std::to_string(5 * scaleTime), cv::Point(i, 10), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1);
    }
}