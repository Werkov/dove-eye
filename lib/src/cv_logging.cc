#include "dove_eye/cv_logging.h"

#include <iostream>
#include <sstream>

#include "config.h"

using std::stringstream;

namespace dove_eye {

static std::map<int, std::string> window_names;

/**
 * @note Not thread-safe!
 */
void log_mat(int id, const cv::Mat &mat) {
#ifdef CONFIG_DEBUG_HIGHGUI
  if (window_names.count(id) == 0) {
    stringstream ss;
    ss << "log_window_" << id;

    window_names[id] = ss.str();
    cv::namedWindow(ss.str(), CV_WINDOW_NORMAL);
  }

  auto window_name = window_names[id];
  cv::imshow(window_name, mat);
#endif
}

/**
 * @note Not thread-safe!
 */
void log_color_hist(int id, const cv::Mat &hist, const int hsize) {
#ifdef CONFIG_DEBUG_HIGHGUI
  using namespace cv;

  Mat histimg = Mat::zeros(200, 320, CV_8UC3);

  histimg = Scalar::all(0);
  int binW = histimg.cols / hsize;
  Mat buf(1, hsize, CV_8UC3);
  for( int i = 0; i < hsize; i++ )
    buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);

  cvtColor(buf, buf, CV_HSV2BGR);

  for( int i = 0; i < hsize; i++ ) {
    int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
    rectangle(histimg, Point(i*binW, histimg.rows),
              Point((i+1)*binW, histimg.rows - val),
              Scalar(buf.at<Vec3b>(i)), -1, 8);
  }

  log_mat(id, histimg);
#endif
}
}
