#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "./videoMeta.hpp"

class Player {
private:
  bool videoLoaded = false;
  bool metaLoaded = false;

  std::ifstream videoFile;
  VideoMeta meta;

  std::string readFrame(int frameNumber);
  void loadCurrentFrameInstance(int second);

public:
  void openVideo(const char* path);
  void loadMeta(VideoMeta& meta);

  void playVideo();
};

std::string removePadding(const std::string& frame) {
    std::istringstream iss(frame);
    std::vector<std::string> lines;
    std::string line;

    // Split into lines
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }

    // Find top and bottom padding
    int top = 0;
    while (top < lines.size() && std::all_of(lines[top].begin(), lines[top].end(), [](char c){ return c == '$' || c == '@'; })) {
        top++;
    }

    int bottom = lines.size() - 1;
    while (bottom >= 0 && std::all_of(lines[bottom].begin(), lines[bottom].end(), [](char c){ return c == '$' || c == '@'; })) {
        bottom--;
    }

    // Find left and right padding
    int left = lines[0].size();
    int right = 0;
    for (int i = top; i <= bottom; i++) {
        int l = 0;
        while (l < lines[i].size() && (lines[i][l] == '$' || lines[i][l] == '@')) l++;
        left = std::min(left, l);

        int r = lines[i].size() - 1;
        while (r >= 0 && (lines[i][r] == '$' || lines[i][r] == '@')) r--;
        right = std::max(right, r);
    }

    // Build trimmed frame
    std::stringstream trimmed;
    for (int i = top; i <= bottom; i++) {
        trimmed << lines[i].substr(left, right - left + 1) << "\n";
    }

    return trimmed.str();
}

std::string Player::readFrame(int frameNumber) {
  videoFile.clear();
  videoFile.seekg(0, std::ios::beg);

  int bytesPerLine = meta.width + 1;
  int offsetLines = frameNumber * meta.height;
  std::streampos offset = offsetLines * bytesPerLine;

  videoFile.seekg(offset, std::ios::beg);

  std::string buffer(meta.height * bytesPerLine, '\0');
  videoFile.read(&buffer[0], meta.height * bytesPerLine);

  buffer.resize(videoFile.gcount());

  return buffer;
}

void Player::loadCurrentFrameInstance(int second) {
  int startingFrame = meta.fps * second;
  int msPerFrame = static_cast<int>((meta.fps / meta.totalFrames) * 1000.00);

  for (int i = 0; i < static_cast<int>(meta.fps); i++) {
    std::string frame = readFrame(startingFrame + i);
    removePadding(frame);
    std::cout << frame;
    std::this_thread::sleep_for(std::chrono::milliseconds(msPerFrame));
    std::cout << "\033[2J\033[H";
  }
}

void Player::openVideo(const char* path) {
  videoFile.open(path, std::ios::in | std::ios::binary);

  if (!videoFile) {
    std::cerr << "error opening file: " << path << "\n";
    exit(1);
  }

  videoLoaded = true;
}

void Player::loadMeta(VideoMeta& meta) {
  this->meta = meta;
  metaLoaded = true;
}

void Player::playVideo() {
  if (!videoLoaded || !metaLoaded) {
    std::cout << "video or meta has not been loaded\n";
    return;
  }

  for (int i = 0; i < static_cast<int>(meta.duration); i++) {
    loadCurrentFrameInstance(i);
  }
}