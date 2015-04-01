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
  InnerTracker::Mark mark(gui_mark.pos.x(), gui_mark.pos.y());
  // TODO encapsulate project_other into GuiMark
  bool project_other = false;
  tracker_->SetMark(cam, mark, project_other);
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

void Controller::SetCalibrationData(const CalibrationData calibration_data) {
  calibration_data_ = std::move(unique_ptr<CalibrationData>(
          new CalibrationData(calibration_data)));
  // TODO set reference to tracker, localization
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

  dove_eye::Location location;
  location.x = 0.3 + 0.4*cos(frameset[0].timestamp * 0.3);
  location.y = 0.3 + 0.7*sin(frameset[0].timestamp * 0.2);
  location.z = 0.1 + 10*sin(frameset[0].timestamp * 0.001);
  emit LocationReady(location);

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

