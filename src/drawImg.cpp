#include "drawImg.hpp"

SpecImage::SpecImage(int height, int width, int inputSam, int numOfCol)
{
    y_ = inputSam / 2 + 1;
    x_ = numOfCol;

    image_ = new cv::Mat(height, width, CV_16UC1, cv::Scalar(0));
    tempImage_ = new cv::Mat(y_, x_, CV_16UC1, cv::Scalar(0));

    if (image_->empty() || tempImage_->empty())
    {
        std::cout << "\n Error - image not created\n";
        return;
    }
}

int SpecImage::createImage(std::vector<float *> data)
{
    int numOfImages = data.size() / x_;
    for (int k = 0; k < numOfImages; ++k)
    {
        for (int i = 0; i < x_; ++i)
        {
            for (int j = 0; j < y_; ++j)
            {
                cv::Point p(i, j);
                //cv::Point p2(2 * i + 1, j);
                cv::Scalar colorIn((data[x_ * k + i][j] / 60 + 1) * 65536);
                cv::line(*tempImage_, p, p, colorIn, 1);
            }
        }
        if (!saveImage(std::to_string(k)))
            return -1;
    }
    return 1;
}

bool SpecImage::saveImage(std::string value)
{
    //cv::resize(*tempImage_, *image_, image_->size(), 0, 0, cv::INTER_CUBIC);
    // bool check = cv::imwrite("./bin/Spectrogram.jpg", *image_);
    bool check = cv::imwrite("./bin/Spectrogram" + value + ".jpg", *tempImage_);

    if (check)
        return true;
    else
        return false;
}