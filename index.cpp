#include "./converter.hpp"
#include "./player.hpp"

int main(int argc, char** argv) {
  std::string videoPath;
  std::cout << "enter video path: ";
  std::cin >> videoPath;

  Converter converter;
  converter.convertVideoToAscii(videoPath.c_str());

  VideoMeta meta = Converter::getVideoMeta("./aura.mp4");

  Player player;
  player.openVideo("./video.txt");
  player.loadMeta(meta);

  player.playVideo();

  return 0;
}