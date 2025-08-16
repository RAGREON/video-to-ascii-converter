#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "./videoMeta.hpp"

class Converter {
private:
  // const std::string gradient = R"($@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,^`'. )";

  const std::string gradient = "@%#*+=-:. ";

public:
  static VideoMeta getVideoMeta(const char* path);

  std::string convertMatToAsciiFrame(cv::Mat* mat);
  std::string convertImageToAsciiFrame(const char* path);
  void convertVideoToAscii(const char* path);
};

VideoMeta Converter::getVideoMeta(const char* path) {
  cv::VideoCapture cap(path);
  if (!cap.isOpened()) {
    std::cerr << "could not load video\n";
    exit(1);
  }

  int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

  int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
  double fps = cap.get(cv::CAP_PROP_FPS);
  double duration = totalFrames / fps;

  VideoMeta meta = {
    100,
    50,
    totalFrames,
    fps,
    duration
  };

  return meta;
}

std::string Converter::convertMatToAsciiFrame(cv::Mat* img) {
  if (img == nullptr) {
    std::cerr << "invalid img";
    exit(1);
  }

  cv::Mat downscale;
  cv::resize(*img, downscale, cv::Size(100, 50), 0, 0, cv::INTER_LINEAR);
 
  int width = downscale.cols;
  int height = downscale.rows;

  cv::Mat gray;
  cv::cvtColor(downscale, gray, cv::COLOR_BGR2GRAY);

  std::stringstream ascii_frame;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      uchar brightness = gray.at<uchar>(y, x);

      int index = static_cast<int>(brightness / 255.0 * (gradient.length() - 1));
      char ascii_character = gradient[index]; 

      ascii_frame << ascii_character;
    }
    
    ascii_frame << "\n";
  }

  return ascii_frame.str();
}

std::string Converter::convertImageToAsciiFrame(const char* path) {
  std::string values = "@%#*+=-:. ";

  cv::Mat img = cv::imread(path);
  if (img.empty()) {
    std::cerr << "could not load image\n";
    exit(1);
  }

  return convertMatToAsciiFrame(&img);
}

void Converter::convertVideoToAscii(const char* path) {
  cv::VideoCapture cap(path);
  if (!cap.isOpened()) {
    std::cerr << "could not load video\n";
    exit(1);
  }

  int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
  double fps = cap.get(cv::CAP_PROP_FPS);
  double duration = totalFrames / fps;

  std::cout << "total frames: " << totalFrames << '\n';
  std::cout << "fps: " << fps << '\n';
  std::cout << "duration: " << duration << '\n';

  std::ofstream asciiVideo("video.txt");

  cv::Mat frame;
  while (true) {
    bool success = cap.read(frame);
    if (!success) {
      std::cout << "end of video or error reading frame\n";
      break;
    }

    std::string asciiFrame = convertMatToAsciiFrame(&frame);
    asciiVideo << asciiFrame;
  }
}