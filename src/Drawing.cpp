#include "Drawing.hpp"

SpecImage::SpecImage(int height, int width, int inputSam, int numOfCol)
{
    y_ = inputSam / 2 + 1; //transform to numer of output samples
    x_ = numOfCol;         // num of columns on image

    image_ = new cv::Mat(height, width, CV_16UC1, cv::Scalar(0));
    //create image based on original data and then scale it
    tempImage_ = new cv::Mat(y_, x_, CV_16UC1, cv::Scalar(0));

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
    return cv::imwrite("./output/Spectrogram" + value + ".png", *image_);
}