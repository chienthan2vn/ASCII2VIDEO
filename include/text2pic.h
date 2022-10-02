#ifndef TEXT_TO_PIC
#define TEXT_TO_PIC

#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Size;

namespace ddk {
  void LoadFont(const std::string &font, const std::u32string &text, int fontSize = 8, float lineScale = 1.0f);
  Mat GenFrame(const Mat& frame);
  void Free();
};

#endif
