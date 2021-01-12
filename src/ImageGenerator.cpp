#include "ImageGenerator.hpp"

ImageGenerator::ImageGenerator(const GeneratorConfiguration *cfg) : RAW_X(cfg->img_fft_per_img_), RAW_Y(cfg->fft_out_frame_count_), SAMPLE_RATE(cfg->out_sample_rate_ / 2), TIME_PER_IMG(cfg->img_time_per_img_), SCALE_TIME_INTERVAL(cfg->img_scale_time_interval_), scale_time_(0)
{
    //image created from original data from FFT
    tempImage_ = new cv::Mat(RAW_Y, RAW_X, CV_16UC1, cv::Scalar(0));

    //output image with declared by user resolution
    image_ = new cv::Mat(cfg->img_height_, cfg->img_width_, CV_16UC1, cv::Scalar(0));

    //output image after adding scale lines
    outputImage_ = new cv::Mat();

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

void ImageGenerator::generateSpectrograms(std::vector<float *> *transforms)
{
    //create images from full windows
    int numOfImages = transforms->size() / RAW_X;
    for (int k = 0; k < numOfImages; ++k)
    {
#pragma omp parallel for
        for (int i = 0; i < RAW_X; ++i)
        {
            for (int j = 0; j < RAW_Y; ++j)
            {
                cv::Point p(i, j);
                cv::Scalar colorIn((transforms->at(RAW_X * k + i)[j] / 60 + 1) * UINT16_MAX); //normalize output from 0 to 65536
                cv::line(*tempImage_, p, p, colorIn, 2);
            }
        }
        if (!saveImage(std::to_string(k)))
            throw std::runtime_error("Error: couldn't save image");
    }

    //create image from remaining samples
    *tempImage_ = cv::Scalar(0); //clear tempImage
#pragma omp parallel for
    for (uint i = 0; i < (transforms->size() - (RAW_X * numOfImages)); ++i)
    {
        for (int j = 0; j < RAW_Y; ++j)
        {
            cv::Point p(i, j);
            cv::Scalar colorIn((transforms->at(RAW_X * numOfImages + i)[j] / 60 + 1) * UINT16_MAX); //normalize output from 0 to 65536
            cv::line(*tempImage_, p, p, colorIn, 2);
        }
    }
    if (!saveImage(std::to_string(numOfImages)))
        throw std::runtime_error("Error: couldn't save image");
}

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
    cv::Scalar colorIn(UINT16_MAX);

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

void ImageGenerator::drawScale(std::string value)
{
    //set point of intersecting scale Lines (upper - right corner)
    const int INTERSECT_X = 45;
    const int INTERSECT_Y = 18;
    //set frequency step on scale
    const int FREQ_OFFSET = 1000;

    int point0[2] = {INTERSECT_X, INTERSECT_Y};
    addScaleLines(point0);

    cv::Scalar colorIn(UINT16_MAX);

    // OY -------------------------------------------------------------------------
    //add axis description
    cv::putText(*outputImage_, "Hz", cv::Point(outputImage_->cols - point0[0] + 6, point0[1] + 10), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1.5);

    //set to draw scale every FREQ_OFFSET
    int stepY = (FREQ_OFFSET * outputImage_->rows) / SAMPLE_RATE;

#pragma omp parallel for
    for (int i = point0[1] + stepY; i < outputImage_->rows; i += stepY)
    {
        cv::line(*outputImage_, cv::Point(outputImage_->cols - point0[0], i), cv::Point(outputImage_->cols - point0[0] + 4, i), colorIn, 1);

        cv::putText(*outputImage_, std::to_string(((i - point0[1]) / stepY) * FREQ_OFFSET), cv::Point(outputImage_->cols - point0[0] + 6, i), cv::FONT_HERSHEY_DUPLEX, 0.3, colorIn, 1);
    }

    //OX --------------------------------------------------------------------------
    //add axis description
    cv::putText(*outputImage_, "ms", cv::Point(outputImage_->cols - point0[0] + 4, point0[1] - 6), cv::FONT_HERSHEY_DUPLEX, 0.4, colorIn, 1);

    //calculate step to draw scale with declared interval
    int stepX = (SCALE_TIME_INTERVAL * outputImage_->cols) / TIME_PER_IMG;
    //calculate how many values program can draw
    int howManyValues = outputImage_->cols / stepX;
    //calculate how many FFT remain
    int remainingFft = outputImage_->cols - stepX * howManyValues;
    int numOfImg = std::stoi(value);

    for (int i = ((numOfImg * remainingFft) % stepX); i < outputImage_->cols - point0[0]; i += stepX, ++scale_time_)
    {

        cv::line(*outputImage_, cv::Point(i, 12), cv::Point(i, 17), colorIn, 1);

        cv::putText(*outputImage_, std::to_string(SCALE_TIME_INTERVAL * scale_time_), cv::Point(i, 10), cv::FONT_HERSHEY_DUPLEX, 0.4, colorIn, 1);
    }
}

cv::Mat *ImageGenerator::getImage()
{
    return image_;
}
cv::Mat *ImageGenerator::getOutputImage()
{
    return outputImage_;
}
cv::Mat *ImageGenerator::getTempImage()
{
    return tempImage_;
}