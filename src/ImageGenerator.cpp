#include "ImageGenerator.hpp"

ImageGenerator::ImageGenerator(const GeneratorConfiguration *cfg)
{
    y_ = cfg->fft_out_frame_count_;
    x_ = cfg->img_fft_per_img_;

    sample_rate_ = cfg->out_sample_rate_ / 2;
    time_per_img_ = cfg->img_time_per_img_;
    scale_time_interval_ = cfg->img_scale_time_interval_;
    scale_time_ = 0;

    //output image with declared by user resolution
    image_ = new cv::Mat(cfg->img_height_, cfg->img_width_, CV_16UC1, cv::Scalar(0));

    //output image after adding scale lines
    outputImage_ = new cv::Mat();

    //image to process based on original data from FFT
    tempImage_ = new cv::Mat(y_, x_, CV_16UC1, cv::Scalar(0));

    if (image_->empty() || tempImage_->empty())
    {
        throw std::runtime_error("Error: couldn`t create image");
    }
}

ImageGenerator::~ImageGenerator()
{
    delete image_;
    delete tempImage_;
    delete outputImage_;
}

//function to create set of images based on data from  FFT
void ImageGenerator::createImage(std::vector<float *> *data)
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

//function to save image into declared by user spectrogram's resolution
bool ImageGenerator::saveImage(std::string value)
{
    //scale tempImage to resolution of output image
    cv::resize(*tempImage_, *image_, image_->size());
    drawScale(value);
    return cv::imwrite("./output/Spectrogram" + value + ".png", *outputImage_);
}

void ImageGenerator::addScaleLines(int point0[])
{
    //init width and height of borders + color
    cv::Scalar background(0);
    cv::Scalar colorIn(65000);

    cv::copyMakeBorder(*image_, *outputImage_, point0[1], 0, 0, point0[0], cv::BORDER_CONSTANT, background);

    //draw OX scale line
    cv::Point beginH(0, point0[1] - 1);
    cv::Point endH(outputImage_->cols, point0[1] - 1);
    cv::line(*outputImage_, beginH, endH, colorIn, 1);

    //draw OY scale line
    cv::Point beginV(outputImage_->cols - point0[0], point0[1]);
    cv::Point endV(outputImage_->cols - point0[0], outputImage_->rows);
    cv::line(*outputImage_, beginV, endV, colorIn, 1);
}

//function to draw scale
void ImageGenerator::drawScale(std::string value)
{
    //set point of intersecting scale Lines (upper - right corner)
    int point0[2] = {45, 18};
    addScaleLines(point0);

    cv::Scalar colorIn(65000);

    // OY
    //add axis describtion
    cv::putText(*outputImage_, "Hz", cv::Point(outputImage_->cols - point0[0] + 6, point0[1] + 10), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1.5);
    //set to draw scale every 1 kHz
    int stepY = (1000 * outputImage_->rows) / sample_rate_;

#pragma omp parallel for
    for (int i = point0[1] + stepY; i < outputImage_->rows; i += stepY)
    {
        cv::line(*outputImage_, cv::Point(outputImage_->cols - point0[0], i), cv::Point(outputImage_->cols - point0[0] + 4, i), colorIn, 1);

        cv::putText(*outputImage_, std::to_string(((i - point0[1]) / stepY) * 1000), cv::Point(outputImage_->cols - point0[0] + 6, i), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1);
    }

    //OX
    //add axis describtion
    cv::putText(*outputImage_, "ms", cv::Point(outputImage_->cols - point0[0] + 4, point0[1] - 6), cv::FONT_HERSHEY_DUPLEX, 0.4, colorIn, 1);
    //calculate step to draw scale with declared interval
    int stepX = (scale_time_interval_ * outputImage_->cols) / time_per_img_;
    //calculate how many values program can draw
    int howManyValues = outputImage_->cols / stepX;
    //calculate how many FFT remain
    int remainingFft = outputImage_->cols - stepX * howManyValues;
    int numOfImg = std::stoi(value);

    for (int i = ((numOfImg * remainingFft) % stepX); i < outputImage_->cols - point0[0]; i += stepX, ++scale_time_)
    {

        cv::line(*outputImage_, cv::Point(i, 12), cv::Point(i, 17), colorIn, 1);

        cv::putText(*outputImage_, std::to_string(scale_time_interval_ * scale_time_), cv::Point(i, 10), cv::FONT_HERSHEY_DUPLEX, 0.4, colorIn, 1);
    }
}