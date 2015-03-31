#include "widgets/calibration_status.h"

#include <cassert>

#include <QHBoxLayout>
#include <QString>

#include "dove_eye/camera_pair.h"

using dove_eye::CameraIndex;
using dove_eye::CameraPair;

namespace widgets {

CalibrationStatus::CalibrationStatus(QWidget *parent)
    : QWidget(parent) {
  auto new_layout = new QHBoxLayout();
  label_ = new QLabel();
  new_layout->addWidget(label_);
  delete layout();
  setLayout(new_layout);

  SetArity(0);
}

void CalibrationStatus::SetArity(const CameraIndex arity) {
  camera_progress_.fill(0, arity);
  pair_progress_.fill(0, CameraPair::Pairity(arity));
}

void CalibrationStatus::CameraCalibrationProgressed(
    const dove_eye::CameraIndex cam,
    const double progress) {
  assert(cam < camera_progress_.size());
  camera_progress_[cam] = progress;

  RefreshStatus();
}

void CalibrationStatus::PairCalibrationProgressed(
    const dove_eye::CameraIndex index,
    const double progress) {
  assert(index < pair_progress_.size());
  pair_progress_[index] = progress;

  RefreshStatus();
}

void CalibrationStatus::RefreshStatus() {
  QString status;

  for (CameraIndex cam = 0; cam < camera_progress_.size(); ++cam) {
    status += QString("Cam %0: %1 %, ").arg(cam)
        .arg(static_cast<int>(camera_progress_[cam] * 100));
  }

  auto pairs = CameraPair::GenerateArray(camera_progress_.size());
  bool first = true;
  for (auto pair : pairs) {
    if (!first) {
      status += ", ";
      first = false;
    }
    status += QString("Pair [%0,%1]: %2 %").arg(pair.cam1).arg(pair.cam2)
        .arg(static_cast<int>(pair_progress_[pair.index] * 100));
  }

  label_->setText(status);
}


} // end namespace widgets
