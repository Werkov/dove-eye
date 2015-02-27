#include <iostream>
#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/camera_calibration.h"
#include "dove_eye/chessboard_pattern.h"
#include "dove_eye/file_video_provider.h"
#include "dove_eye/frameset_aggregator.h"
#include "dove_eye/logging.h"
#include "dove_eye/time_calibration.h"
#include "dove_eye/types.h"
#include "dove_eye/video_provider.h"

using cv::FileStorage;

using dove_eye::BlockingPolicy;
using dove_eye::CameraCalibration;
using dove_eye::CameraIndex;
using dove_eye::ChessboardPattern;
using dove_eye::FileVideoProvider;
using dove_eye::FramesetAggregator;
using dove_eye::TimeCalibration;
using dove_eye::VideoProvider;

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::vector;

namespace {

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

int CalibrateCameras(const string &output, const vector<string> &filenames) {
  typedef FramesetAggregator<BlockingPolicy> Aggregator;
  cv::namedWindow("test");

  // TODO this works, consider adding virtual dtor to CalibrationPattern though
  ChessboardPattern pattern(6, 9, 0.026); // inner corners, 26 mm
  CameraCalibration calibration(filenames.size(), pattern);

  BlockingPolicy::ProvidersContainer providers;
  Aggregator::OffsetsContainer offsets;

  for (auto filename : filenames) {
    providers.push_back(unique_ptr<VideoProvider>(
            new FileVideoProvider(filename)));
    offsets.push_back(0);
  }


  // TODO window size should maximum offset
  Aggregator aggregator(
      std::move(providers), offsets, 2.5);

  for (auto frameset : aggregator) {
    if (calibration.MeasureFrameset(frameset)) {
      break;
    }
  }

  FileStorage file_storage(output, FileStorage::WRITE);
  if (!file_storage.isOpened()) {
    ERROR("Cannot open file %s\n", output.c_str());
    return 1;
  }

  file_storage << "camera_count" << calibration.camera_count();
  file_storage << "pair_count" << static_cast<int>(calibration.pairs().size());

  for (CameraIndex cam = 0; cam < calibration.camera_count(); ++cam) {
    file_storage << (string("cam") + to_string(cam)) << "{" <<
        "C" << calibration.camera_result(cam).camera_matrix <<
        "D" << calibration.camera_result(cam).distortion_coefficients <<
        "}";
  }

  for (auto pair : calibration.pairs()) {
    file_storage << (string("pair") + to_string(pair.index)) << "{" <<
        "E" << calibration.pair_result(pair.cam1, pair.cam2).essential_matrix <<
        "}";
  }

  file_storage.release();

  return 0;
}

int TestVideo(const vector<string> &filenames) {
  cv::namedWindow("test");

  for (auto filename : filenames) {
    FileVideoProvider provider(filename);
    for (auto frame : provider) {
      cv::imshow("test", frame.data);
      cv::waitKey(-1);
    }
  }


  return 0;
}

static void PrintUsage(const string &name) {
  cout << "Usage: " << name << " [-t|-c] outfile video-file ..." << endl;
}

} // namespace

int main(int argc, char* argv[]) {
  string name(argv[0]);
  ++argv;
  --argc;

  if (argc < 3) {
    PrintUsage(name);
    return 1;
  }

  if (string(argv[0]) == "-t") {
    ++argv;
    --argc;

    return CalibrateTime(vector<string>(argv, argv + argc));
  } else if (string(argv[0]) == "-c") {
    ++argv;
    --argc;

    return CalibrateCameras(argv[0], vector<string>(argv + 1, argv + argc));
  } else {
    PrintUsage(name);
    return 1;
  }
}

