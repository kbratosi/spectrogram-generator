/**
 * @file ImageGenerator.cpp
 * @author Jakub Marczuk
 * @brief File containing member functions of ImageGenerator
 * @version 1.0
 * @date 2021-01-12
 */

#include "ImageGenerator.hpp"

ImageGenerator::ImageGenerator(const GeneratorConfiguration *cfg) : 
  RAW_X(cfg->img_fft_per_img_),
  RAW_Y(cfg->fft_out_frame_count_), SAMPLE_RATE(cfg->out_sample_rate_ / 2),
  TIME_PER_IMG(cfg->img_time_per_img_),
  SCALE_TIME_INTERVAL(cfg->img_scale_time_interval_),
  scale_time_(0)
{
  //image created from original data from FFT
  temp_image_ = new cv::Mat(RAW_Y, RAW_X, CV_16UC1, cv::Scalar(0));

  //output image with declared by user resolution
  image_ = new cv::Mat(cfg->img_height_, cfg->img_width_, CV_16UC1, cv::Scalar(0));

  //output image after adding scale lines
  output_image_ = new cv::Mat();

  if (image_->empty() || temp_image_->empty())
  {
    throw std::runtime_error("Error: couldn`t create image");
  }
}

ImageGenerator::~ImageGenerator()
{
  delete image_;
  delete temp_image_;
  delete output_image_;
}

void ImageGenerator::generateSpectrograms(std::vector<float *> *transforms)
{
  //create images from full windows
  int n_images = transforms->size() / RAW_X;
  for (int k = 0; k < n_images; ++k)
  {
#pragma omp parallel for
    for (int i = 0; i < RAW_X; ++i)
    {
      for (int j = 0; j < RAW_Y; ++j)
      {
        cv::Point p(i, j);
        //normalize output from 0 to 65536
        cv::Scalar color_in((transforms->at(RAW_X * k + i)[j] / 60 + 1) * UINT16_MAX);
        cv::line(*temp_image_, p, p, color_in, 2);
      }
    }
    if (!saveImage(std::to_string(k)))
      throw std::runtime_error("Error: couldn't save image");
  }

  //create image from remaining samples
  *temp_image_ = cv::Scalar(0); //clear temp_image
#pragma omp parallel for
  for (uint i = 0; i < (transforms->size() - (RAW_X * n_images)); ++i)
  {
    for (int j = 0; j < RAW_Y; ++j)
    {
      cv::Point p(i, j);
      //normalize output from 0 to 65536
      cv::Scalar color_in((transforms->at(RAW_X * n_images + i)[j] / 60 + 1) * UINT16_MAX); 
      cv::line(*temp_image_, p, p, color_in, 2);
    }
  }
  if (!saveImage(std::to_string(n_images)))
    throw std::runtime_error("Error: couldn't save image");
}

bool ImageGenerator::saveImage(std::string value)
{
  //scale temp_image to resolution of output_image
  cv::resize(*temp_image_, *image_, image_->size());
  drawScale(value);
  return cv::imwrite("./output/Spectrogram" + value + ".png", *output_image_);
}

void ImageGenerator::addScaleLines(int point0[])
{
  //init width and height of borders + color
  cv::Scalar background(0);
  cv::Scalar color_in(UINT16_MAX);

  cv::copyMakeBorder(*image_, *output_image_, point0[1], 0, 0, point0[0], cv::BORDER_CONSTANT, background);

  //draw OX scale line
  cv::Point begin_h(0, point0[1] - 1);
  cv::Point end_h(output_image_->cols, point0[1] - 1);
  cv::line(*output_image_, begin_h, end_h, color_in, 1);

  //draw OY scale line
  cv::Point begin_v(output_image_->cols - point0[0], point0[1]);
  cv::Point end_v(output_image_->cols - point0[0], output_image_->rows);
  cv::line(*output_image_, begin_v, end_v, color_in, 1);
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

  cv::Scalar color_in(UINT16_MAX);

  // OY -------------------------------------------------------------------------
  // vertical axis - frequency domain
  cv::putText(*output_image_, "Hz", cv::Point(output_image_->cols - point0[0] + 6, point0[1] + 10), cv::FONT_HERSHEY_DUPLEX, 0.3, color_in, 1.5);

  //set to draw scale every FREQ_OFFSET
  int step_y = (FREQ_OFFSET * output_image_->rows) / SAMPLE_RATE;

#pragma omp parallel for
  for (int i = point0[1] + step_y; i < output_image_->rows; i += step_y)
  {
    cv::line(*output_image_, cv::Point(output_image_->cols - point0[0], i), cv::Point(output_image_->cols - point0[0] + 4, i), color_in, 1);
    cv::putText(*output_image_, std::to_string(((i - point0[1]) / step_y) * FREQ_OFFSET), cv::Point(output_image_->cols - point0[0] + 6, i), cv::FONT_HERSHEY_DUPLEX, 0.3, color_in, 1);
  }

  //OX --------------------------------------------------------------------------
  // horizontal axis - time domain
  cv::putText(*output_image_, "ms", cv::Point(output_image_->cols - point0[0] + 4, point0[1] - 6), cv::FONT_HERSHEY_DUPLEX, 0.4, color_in, 1);

  //calculate step to draw scale with declared interval
  int step_x = (SCALE_TIME_INTERVAL * output_image_->cols) / TIME_PER_IMG;
  //calculate how many values program can draw
  int n_values = output_image_->cols / step_x;
  //calculate how many FFT remain
  int remaining_transforms = output_image_->cols - step_x * n_values;
  int n_images = std::stoi(value);

  for (int i = ((n_images * remaining_transforms) % step_x); i < output_image_->cols - point0[0]; i += step_x, ++scale_time_)
  {
    cv::line(*output_image_, cv::Point(i, 12), cv::Point(i, 17), color_in, 1);
    cv::putText(*output_image_, std::to_string(SCALE_TIME_INTERVAL * scale_time_), cv::Point(i, 10), cv::FONT_HERSHEY_DUPLEX, 0.4, color_in, 1);
  }
}

cv::Mat *ImageGenerator::getImage()
{
  return image_;
}
cv::Mat *ImageGenerator::getOutputImage()
{
  return output_image_;
}
cv::Mat *ImageGenerator::getTempImage()
{
  return temp_image_;
}