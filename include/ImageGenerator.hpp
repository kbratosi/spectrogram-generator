/**
 * @file ImageGenerator.hpp
 * @author Jakub Marczuk
 * @brief File containing ImageGenerator class
 * @version 1.0
 * @date 2021-01-12
 */

#ifndef IMAGEGENERATOR_HPP
#define IMAGEGENERATOR_HPP

#include "GeneratorConfiguration.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <omp.h>

/// Create set of spectrograms
class ImageGenerator
{
public:
  /// @param cfg structure holding parameters required during spectrogram generation process
  /// @see GeneratorConfiguration
  ImageGenerator(const GeneratorConfiguration *cfg);
  ~ImageGenerator();
  /// save image into declared by user spectrogram's resolution
  /// @param value information about order of created spectrograms
  bool saveImage(std::string value);
  /// create set of images based on data from FFT
  /// @param data vector which stores processed data in frequency domain
  void generateSpectrograms(std::vector<float *> *transforms);
  /// draw scale on image
  /// @param value information about presently created spectrogram to scale properly time scale line
  /// @note color intensity is calculated by normalizing transform to range <0;1> and multiplying it by maximum intensity
  void drawScale(std::string value);
  /// draw scale lines on image
  /// @param point0 array which stores intersection point of scale lines
  void addScaleLines(int point0[]);

  cv::Mat *getImage();
  cv::Mat *getOutputImage();
  cv::Mat *getTempImage();

private:
  const int RAW_X,               // width of image created from FFT transformation
            RAW_Y,               // height of image created from FFT transformation
            SAMPLE_RATE,         // sampling frequency of processed audio data
            TIME_PER_IMG,        // time period displayed on a single spectrogram
            SCALE_TIME_INTERVAL; // horizontal scale division unit
  //
  int scale_time_;               // variable used to help displaying value on scale
  cv::Mat *image_, *output_image_, *temp_image_;
};

#endif