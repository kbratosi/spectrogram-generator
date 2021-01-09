#include "Drawing.hpp"

SpecImage::SpecImage(int height, int width, int inputSam, int numOfCol, int samplingRate, float timePerImg)
{
    y_ = inputSam / 2 + 1; //transform to numer of output samples
    x_ = numOfCol;         // num of columns on image
    samplingRate_ = samplingRate / 2;
    imgResolution[0] = width;
    imgResolution[1] = height;
    timePerImg_ = timePerImg;
    scaleTime = 0;

    //output image
    image_ = new cv::Mat(height, width, CV_16UC1, cv::Scalar(0));
    //create temporary image based on original data and then scale it
    tempImage_ = new cv::Mat(y_, x_, CV_16UC1, cv::Scalar(0));

    //TODO correct handling errors
    if (image_->empty() || tempImage_->empty())
    {
        std::cout << "\n Error - image not created\n";
        return;
    }
}

SpecImage::~SpecImage()
{
    delete image_;
    delete tempImage_;
}

int SpecImage::createImage(std::vector<float *> *data)
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
            return -1;
    }

    //create image from remaining samples
    //clear tempImage
    *tempImage_ = cv::Scalar(0);
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
        return -1;

    return 1;
}

bool SpecImage::saveImage(std::string value)
{
    cv::resize(*tempImage_, *image_, image_->size());
    drawScale(value);
    return cv::imwrite("./output/Spectrogram" + value + ".png", *image_);
}

void SpecImage::addBorder()
{
    //init width and height of borders + color
    //int top = (int)(0.02 * image_->rows);
    //int left = (int)(0.015 * image_->cols);
    int top = 18, left = 25;
    cv::Scalar background(0);

    cv::copyMakeBorder(*image_, *image_, top, 0, left, 0, cv::BORDER_CONSTANT, background);
}

void SpecImage::drawScale(std::string value)
{
    addBorder();

    //set point from which begins spectrogram
    //int point0[2] = {0.015 * image_->cols, 0.02 * image_->rows};
    int point0[2] = {25, 18};

    cv::Point beginH(0, point0[1] - 1);
    cv::Point endH(image_->cols, point0[1] - 1);
    cv::Point beginV(point0[0] - 1, 0);
    cv::Point endV(point0[0] - 1, image_->rows);
    cv::Scalar colorIn(65000);

    //draw number lines
    cv::line(*image_, beginH, endH, colorIn, 1);
    cv::line(*image_, beginV, endV, colorIn, 1);

    // OY
    //set to draw scale every 1 kHz
    int stepY = (1000 * image_->rows) / samplingRate_;
#pragma omp parallel for
    for (int i = point0[1]; i < image_->rows; i += stepY)
    {
        cv::putText(*image_, std::to_string(((i - point0[1]) / stepY) * 1000), cv::Point(0, i), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1);
    }

    //OX
    //set to draw scale every 5 seconds
    int stepX = (5 * image_->cols) / timePerImg_;
    int howManyValues = image_->cols / stepX;
    int remainingFft = image_->cols - stepX * howManyValues;
    int numOfImg = std::stoi(value);

    for (int i = ((numOfImg * remainingFft) % stepX) + point0[0]; i < image_->cols; i += stepX, ++scaleTime)
    {
        cv::putText(*image_, std::to_string(5 * scaleTime), cv::Point(i, 10), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1);
    }
}