#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

class Nox {
private:
  int width;
  int height;

  std::string videoPath;
  // const std::string gradient = R"($@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,^`'. )";
  const std::string gradient = "@%#*+=-:. ";
  
  std::string convertMatToAsciiFrame(cv::Mat* mat);

public:
  Nox(const char* path, int width, int height) : width(width), height(height) {
    videoPath = path;
  }
  
  void videoToAscii();
};

std::string Nox::convertMatToAsciiFrame(cv::Mat* img) {
  if (img == nullptr) {
    std::cerr << "invalid img";
    exit(1);
  }

  cv::Mat downscale;
  cv::resize(*img, downscale, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
 
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

void Nox::videoToAscii() { 
  cv::VideoCapture cap(videoPath);
  if (!cap.isOpened()) {
    std::cerr << "could not load video\n";
    exit(1);
  }

  int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
  double fps = cap.get(cv::CAP_PROP_FPS);

  int msPerFrame = static_cast<int>((fps / totalFrames) * 1000.00);

  cv::Mat frame;
  while (true) {
    bool success = cap.read(frame);
    if (!success) {
      std::cout << "end of video or error reading frame\n";
      break;
    }

    std::string asciiFrame = convertMatToAsciiFrame(&frame);
    std::cout << asciiFrame;

    std::this_thread::sleep_for(std::chrono::milliseconds(msPerFrame));
    std::cout << "\033[2J\033[H";
  }
}