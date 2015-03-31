#include "widgets/controller_status.h"

#include <QHBoxLayout>
#include <QString>

namespace widgets {

ControllerStatus::ControllerStatus(QWidget *parent)
    : QWidget(parent) {
  auto new_layout = new QHBoxLayout();
  label_ = new QLabel();
  new_layout->addWidget(label_);

  delete layout();
  setLayout(new_layout);
}

void ControllerStatus::ModeChanged(const Controller::Mode mode) {
  QString status;

  switch (mode) {
    case Controller::kIdle:
      status = "Idle";
      break;
    case Controller::kCalibration:
      status = "Calibration";
      break;
    case Controller::kTracking:
      status = "Tracking";
      break;
    case Controller::kNonexistent:
      status = "---";
      break;
  }

  label_->setText(status);
}


} // end namespace widgets
