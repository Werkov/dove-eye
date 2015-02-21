#include <iostream>
#include <unistd.h>
#include <vector>

#include <opencv2/opencv.hpp>

#include <dove_eye/camera_calibration.h>
#include <dove_eye/chessboard_pattern.h>
#include <dove_eye/file_video_provider.h>
#include <dove_eye/time_calibration.h>

using dove_eye::CameraCalibration;
using dove_eye::ChessboardPattern;
using dove_eye::FileVideoProvider;
using dove_eye::TimeCalibration;

using std::string;
using std::vector;

int CalibrateTime(const vector<string> &filenames) {
  TimeCalibration calibration;
  vector<TimeCalibration::ResultType> result;

  for (auto filename : filenames) {
    FileVideoProvider provider(filename);
    calibration.Reset();

    for (auto frame : provider) {
      if (calibration.MeasureFrame(frame)) {
        break;
      }
    }

    if (calibration.state() == TimeCalibration::kReady) {
      std::cout << filename << ": " << calibration.Result() << std::endl;
    } else {
      std::cout << filename << ": NA" << std::endl;
    }
  }

  // TODO store serialized result           
  return 0;
}

int CalibrateCameras(const vector<string> &filenames) {
  cv::namedWindow("test");
  // TODO this works, consider adding virtual dtor to CalibrationPattern though
  ChessboardPattern pattern(6, 9, 0.026); // inner corners, 26 mm
  CameraCalibration calibration(filenames.size(), pattern);
  for (auto filename : filenames) {

    FileVideoProvider provider(filename);

    for (auto frame : provider) {
      cv::imshow("test", frame.data);
      cv::waitKey(0);
    }


  }

  // TODO store serialized result           
  return 0;
}

int main(int argc, char* argv[]) {
  string usage = "Usage: %0 [-t|-c]";
 
  return CalibrateCameras(vector<string>(argv + 1, argv + argc));
  //return CalibrateTime(vector<string>(argv + 1, argv + argc));
}

