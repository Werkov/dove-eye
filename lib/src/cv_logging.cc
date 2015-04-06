#include "dove_eye/cv_logging.h"

#include <sstream>

using std::stringstream;

namespace dove_eye {

static std::map<int, std::string> window_names;

/**
 * @note Not thread-safe!
 */
void log_mat(int id, const cv::Mat &mat) {
#ifndef NDEBUG
  if (window_names.count(id) == 0) {
    stringstream ss;
    ss << "log_window_" << id;

    window_names[id] = ss.str();
    cv::namedWindow(ss.str(), CV_WINDOW_NORMAL);
  }

  auto window_name = window_names[id];
  cv::imshow(window_name, mat);
  //cv::waitKey(1);
#endif
}
}
