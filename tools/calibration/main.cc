#include <iostream>
#include <unistd.h>
#include <vector>

#include <dove_eye/file_video_provider.h>

#include "dove_eye/time_calibration.h"

using std::string;
using std::vector;
using dove_eye::TimeCalibration;
using dove_eye::FileVideoProvider;

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
      std::cout << filename << ": " << calibration.result() << std::endl;
    } else {
      std::cout << filename << ": NA" << std::endl;
    }
  }

  // TODO store serialized result           
}

int main(int argc, char* argv[]) {
  string usage = "Usage: %0 [-t|-c]";

  return CalibrateTime(vector<string>(argv + 1, argv + argc));

}

