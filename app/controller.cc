#include "controller.h"

#include <cassert>

#include <opencv2/opencv.hpp>
#include <QTimerEvent>

#include "dove_eye/inner_tracker.h"

using dove_eye::CalibrationData;
using dove_eye::CameraIndex;
using dove_eye::Frameset;
using dove_eye::InnerTracker;
using dove_eye::Parameters;
using gui::GuiMark;
using std::unique_ptr;


void Controller::Start() {
  frameset_iterator_ = aggregator_->begin();
  frameset_end_iterator_ = aggregator_->end();

  SetMode(kIdle);
  timer_.start(0, this);
}

void Controller::Stop() {
  timer_.stop();
}

void Controller::SetMark(const dove_eye::CameraIndex cam,
                         const GuiMark gui_mark) {
  if (!calibration_data_) {
    return;
  }

  InnerTracker::Mark mark(gui_mark.pos.x(), gui_mark.pos.y());
  bool project_other = false;
  if (gui_mark.flags & GuiMark::kCtrl) {
    project_other = true;
  }

  if (tracker_->SetMark(cam, mark, project_other)) {
    SetMode(kTracking);
  }
}

void Controller::SetMode(const Mode mode) {
  mode_ = mode;

  switch (mode_) {
    case kCalibration:
      calibration_->Reset();
      break;
    default:
      /* empty */
      break;
  }

  emit ModeChanged(mode_);
}

void Controller::SetUndistortMode(const UndistortMode undistort_mode) {
  undistort_mode_ = undistort_mode;

  switch (undistort_mode_) {
    case kIgnoreDistortion:
      tracker_->distorted_input(false);
      UndistortToProviders(false);
      break;

    case kUndistortVideo:
      tracker_->distorted_input(false);
      UndistortToProviders(true);

      break;
    case kUndistortData:
      tracker_->distorted_input(true);
      UndistortToProviders(false);
      break;
  }
}

void Controller::SetCalibrationData(const CalibrationData calibration_data) {
  /* Before we delete old calibration_data update references. */
  auto new_calibration_data = new CalibrationData(calibration_data);

  assert(tracker_);
  tracker_->calibration_data(new_calibration_data);

  CalibrationDataToProviders(new_calibration_data);

  // TODO set reference to localization

  calibration_data_ =
      std::move(unique_ptr<CalibrationData>(new_calibration_data));
}

void Controller::timerEvent(QTimerEvent *event) {
  if (event->timerId() != timer_.timerId()) {
    return;
  }

  if (frameset_iterator_ == frameset_end_iterator_) {
    timer_.stop();
    return;
  }

  auto frameset = *frameset_iterator_;
  dove_eye::Positset positset(Arity());

  switch (mode_) {
    case kIdle:
      break;
    case kCalibration:
      if (calibration_->MeasureFrameset(frameset)) {
        SetMode(kIdle);
        /*
         * This will notify the application and it will signal back to us,
         * to update tracker, etc.
         */
        emit CalibrationDataReady(calibration_->Data());
        break;
      }

      for (CameraIndex cam = 0; cam < Arity(); ++cam) {
        emit CameraCalibrationProgressed(cam,
                                         calibration_->CameraProgress(cam));
      }
      for (auto pair : calibration_->pairs()) {
        emit PairCalibrationProgressed(pair.index,
                                       calibration_->PairProgress(pair.index));
      }

      break;
    case kTracking: {
      positset = tracker_->Track(frameset);
      emit PositsetReady(positset);

      auto location = localization_->Locate(positset);
      emit LocationReady(location);
      break;
    }
    case kNonexistent:
      assert(false);
      break;
  }


  DecorateFrameset(frameset, positset);

  ++frameset_iterator_;
}

void Controller::DecorateFrameset(dove_eye::Frameset &frameset,
                                  const dove_eye::Positset positset) {
  for (CameraIndex cam = 0; cam < frameset.Arity(); ++cam) {
    if (!frameset.IsValid(cam)) {
      continue;
    }

    if (positset.IsValid(cam)) {
      cv::circle(frameset[cam].data, positset[cam],
                 parameters_.Get(Parameters::TEMPLATE_RADIUS),
                 cv::Scalar(0, 255, 0));
    }
  }

  emit FramesetReady(*frameset_iterator_);
}

void Controller::CalibrationDataToProviders(
    const CalibrationData *calibration_data) {

  CameraIndex cam = 0;
  for (auto provider : aggregator_->providers()) {
    provider->camera_parameters(&calibration_data->camera_parameters(cam));
    ++cam;
  }
}

void Controller::UndistortToProviders(const bool undistort) {
  for (auto provider : aggregator_->providers()) {
    provider->undistort(undistort);
  }
}

