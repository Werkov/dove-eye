#include "main_window.h"

#include <cassert>

#include <QtDebug>

#include "controller.h"
#include "dove_eye/types.h"
#include "frameset_viewer.h"
#include "ui_main_window.h"

using dove_eye::CalibrationData;
using dove_eye::CameraIndex;
using dove_eye::Parameters;
using gui::FramesetViewer;
using widgets::CalibrationStatus;
using widgets::ControllerStatus;

namespace gui {

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent),
      application_(application),
      ui_(new Ui::MainWindow),
      parameters_dialog_(new ParametersDialog(application->parameters())),
      cameras_setup_dialog_(new CamerasSetupDialog()) {
  ui_->setupUi(this);
  CreateStatusBar();

  connect(application_, &Application::SetupPipeline,
          this, &MainWindow::SetupPipeline);
  connect(application_, &Application::CalibrationDataReady,
          this, &MainWindow::CalibrationDataReady);

  /* Dialog connections */
  connect(cameras_setup_dialog_.get(), &CamerasSetupDialog::SelectedProviders,
          application_, &Application::UseCameraProviders);

  /* Menu actions */
  connect(ui_->action_abort_calibration, &QAction::triggered,
          this, &MainWindow::AbortCalibration);
  connect(ui_->action_calibrate, &QAction::triggered,
          this, &MainWindow::Calibrate);
  connect(ui_->action_modify_parameters, &QAction::triggered,
          this, &MainWindow::ModifyParameters);
  connect(ui_->action_setup_cameras, &QAction::triggered,
          this, &MainWindow::SetupCameras);

  /* Initialization */
  SetupPipeline();
}

MainWindow::~MainWindow() {
}

void MainWindow::SetupPipeline() {
  ui_->viewer->SetArity(application_->Arity());
  calibration_status_->SetArity(application_->Arity());

  if (application_->Arity() == 0) {
    SetCalibration(false);
    ControllerModeChanged(Controller::kNonexistent);
    controller_status_->ModeChanged(Controller::kNonexistent);
    return;
  }

  ControllerModeChanged(application_->controller()->mode());
  controller_status_->ModeChanged(application_->controller()->mode());

  ui_->viewer->SetConverter(application_->converter());

  /* Connect new converter */
  connect(application_->converter(), &FramesetConverter::ImagesetReady,
                   ui_->viewer, &FramesetViewer::SetImageset);

  /* Connect new controller */
  connect(application_->controller(), &Controller::ModeChanged,
          this, &MainWindow::ControllerModeChanged);
  connect(application_->controller(), &Controller::ModeChanged,
          controller_status_, &ControllerStatus::ModeChanged);


  connect(application_->controller(), &Controller::CameraCalibrationProgressed,
          calibration_status_, &CalibrationStatus::CameraCalibrationProgressed);

  connect(application_->controller(), &Controller::PairCalibrationProgressed,
          calibration_status_, &CalibrationStatus::PairCalibrationProgressed);

}

void MainWindow::CalibrationDataReady(const CalibrationData data) {
  SetCalibration(true);
}

void MainWindow::AbortCalibration() {
  assert(application_->controller());

  application_->controller()->SetMode(Controller::kIdle);
}

void MainWindow::Calibrate() {
  assert(application_->controller());

  application_->controller()->SetMode(Controller::kCalibration);
}

void MainWindow::ModifyParameters() {
  parameters_dialog_->LoadValues();
  parameters_dialog_->show();
}

void MainWindow::SetupCameras() {
  cameras_setup_dialog_->SetProviders(application_->AvailableVideoProviders());
  cameras_setup_dialog_->show();
}

void MainWindow::ControllerModeChanged(const Controller::Mode mode) {
  /* Update menu */
  ui_->action_abort_calibration->setVisible(mode == Controller::kCalibration);
  ui_->action_calibrate->setVisible(mode != Controller::kCalibration);
  ui_->action_calibrate->setEnabled(mode != Controller::kNonexistent);

  /* Update status bar */
  bool show_calibration = (mode == Controller::kCalibration);
  if (show_calibration) {
    statusBar()->addWidget(calibration_status_);
    calibration_status_->show();
  } else {
    statusBar()->removeWidget(calibration_status_);
  }
}

void MainWindow::SetCalibration(const bool value) {
  ui_->action_localization_start->setEnabled(value);
}

void MainWindow::CreateStatusBar() {
  controller_status_ = new ControllerStatus();
  statusBar()->addPermanentWidget(controller_status_);

  calibration_status_ = new CalibrationStatus();
  statusBar()->addWidget(calibration_status_);
}

} // end namespace gui
