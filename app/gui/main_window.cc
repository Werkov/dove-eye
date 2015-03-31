#include "main_window.h"

#include <cassert>

#include "controller.h"
#include "dove_eye/types.h"
#include "frameset_viewer.h"
#include "ui_main_window.h"

using dove_eye::CameraIndex;
using dove_eye::Parameters;
using gui::FramesetViewer;
using widgets::CalibrationStatus;

namespace gui {

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent),
      application_(application),
      ui_(new Ui::MainWindow),
      parameters_dialog_(new ParametersDialog(application->parameters())),
      video_providers_dialog_(new VideoProvidersDialog()) {
  ui_->setupUi(this);
  CreateStatusBar();

  connect(application_, &Application::ChangedArity,
          this, &MainWindow::ChangeArity);

  /* Dialog connections */
  connect(video_providers_dialog_.get(), &VideoProvidersDialog::SelectedProviders,
          application_, &Application::UseVideoProviders);

  /* Menu actions */
  connect(ui_->action_calibrate, &QAction::triggered,
          this, &MainWindow::Calibrate);
  connect(ui_->action_modify_parameters, &QAction::triggered,
          this, &MainWindow::ModifyParameters);
  connect(ui_->action_video_providers, &QAction::triggered,
          this, &MainWindow::VideoProviders);

  /* Initialization */
  ChangeArity(application_->Arity());
}

MainWindow::~MainWindow() {
}

void MainWindow::ChangeArity(const CameraIndex arity) {
  ui_->viewer->SetArity(arity);
  calibration_status_->SetArity(arity);

  if (arity == 0) {
    // TODO calibration is disabled when runnig (or there should be a stop)
    ui_->action_calibrate->setEnabled(false);
    return;
  }

  ui_->action_calibrate->setEnabled(true);

  ui_->viewer->SetConverter(application_->converter());

  /* Connect new converter */
  connect(application_->converter(), &FramesetConverter::ImagesetReady,
                   ui_->viewer, &FramesetViewer::SetImageset);

  /* Connect new controller */
  connect(application_->controller(), &Controller::ModeChanged,
          this, &MainWindow::ControllerModeChanged);

  connect(application_->controller(), &Controller::CameraCalibrationProgressed,
          calibration_status_, &CalibrationStatus::CameraCalibrationProgressed);

  connect(application_->controller(), &Controller::PairCalibrationProgressed,
          calibration_status_, &CalibrationStatus::PairCalibrationProgressed);

}

void MainWindow::Calibrate() {
  assert(application_->controller());

  application_->controller()->SetMode(Controller::kCalibration);
}

void MainWindow::ModifyParameters() {
  parameters_dialog_->LoadValues();
  parameters_dialog_->show();
}

void MainWindow::VideoProviders() {
  video_providers_dialog_->SetProviders(application_->AvailableVideoProviders());
  video_providers_dialog_->show();
}

void MainWindow::ControllerModeChanged(const Controller::Mode mode) {
  switch (mode) {
    case Controller::kIdle:
      statusBar()->removeWidget(calibration_status_);
      break;
    case Controller::kCalibration:
      statusBar()->addWidget(calibration_status_);
      calibration_status_->show();
      break;
    case Controller::kTracking:
      statusBar()->removeWidget(calibration_status_);
      break;
  }
}

void MainWindow::CreateStatusBar() {
  calibration_status_ = new CalibrationStatus();
  statusBar()->addWidget(calibration_status_);
}

} // end namespace gui
