#include "main_window.h"

#include <cassert>

#include <QFileDialog>
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
using widgets::SceneViewer;

namespace gui {

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent),
      application_(application),
      ui_(new Ui::MainWindow),
      parameters_dialog_(new ParametersDialog(application->parameters())),
      cameras_setup_dialog_(new CamerasSetupDialog()) {
  ui_->setupUi(this);
  SetupStatusBar();

  connect(application_, &Application::SetupPipeline,
          this, &MainWindow::SetupPipeline);
  connect(application_, &Application::CalibrationDataReady,
          this, &MainWindow::CalibrationDataReady);
  connect(application_, &Application::CalibrationDataReady,
          ui_->scene_viewer, &SceneViewer::SetCalibrationData);

  /* Dialog connections */
  connect(cameras_setup_dialog_.get(), &CamerasSetupDialog::SelectedProviders,
          application_, &Application::UseCameraProviders);

  SetupMenu();

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
  assert(application_->controller());

  ControllerModeChanged(application_->controller()->mode());
  controller_status_->ModeChanged(application_->controller()->mode());

  ui_->viewer->SetConverter(application_->converter());

  /* Connect new converter */
  connect(application_->converter(), &FramesetConverter::ImagesetReady,
                   ui_->viewer, &FramesetViewer::SetImageset);

  /* Connect new controller
   * Note the controller is running its own thread, therefore any communication
   * with it must be done via signals only.
   */
  connect(application_->controller(), &Controller::ModeChanged,
          this, &MainWindow::ControllerModeChanged);
  connect(application_->controller(), &Controller::ModeChanged,
          controller_status_, &ControllerStatus::ModeChanged);


  connect(application_->controller(), &Controller::CameraCalibrationProgressed,
          calibration_status_, &CalibrationStatus::CameraCalibrationProgressed);

  connect(application_->controller(), &Controller::PairCalibrationProgressed,
          calibration_status_, &CalibrationStatus::PairCalibrationProgressed);

  connect(application_->controller(), &Controller::LocationReady,
          ui_->scene_viewer, &SceneViewer::SetLocation);


  connect(this, &MainWindow::SetControllerMode,
          application_->controller(), &Controller::SetMode);
  connect(this, &MainWindow::SetUndistortMode,
          application_->controller(), &Controller::SetUndistortMode);
}

void MainWindow::CalibrationDataReady(const CalibrationData data) {
  SetCalibration(true);
}

void MainWindow::AbortCalibration() {
  emit SetControllerMode(Controller::kIdle);
}

void MainWindow::Calibrate() {
  emit SetControllerMode(Controller::kCalibration);
}

void MainWindow::CalibrationLoad() {
  auto filename = QFileDialog::getOpenFileName(this, tr("Load calibration"), "",
                                               tr("YAML files (*.yaml)"));
  if (filename.isNull()) {
    return;
  }
  auto calibration_data = application_->calibration_data_storage()
      ->LoadFromFile(filename);

  application_->SetCalibrationData(calibration_data);
}

void MainWindow::CalibrationSave() {
  auto filename = QFileDialog::getSaveFileName(this, tr("Save calibration"), "",
                                               tr("YAML files (*.yaml)"));
  if (filename.isNull()) {
    return;
  }

  application_->calibration_data_storage()
      ->SaveToFile(filename, application_->calibration_data());
}

void MainWindow::GroupDistortion(QAction *action) {
  if (action == ui_->action_distortion_ignore) {
    emit SetUndistortMode(Controller::kIgnoreDistortion);
  } else if (action == ui_->action_distortion_video) {
    emit SetUndistortMode(Controller::kUndistortVideo);
  } else if (action == ui_->action_distortion_data) {
    emit SetUndistortMode(Controller::kUndistortData);
  }
}

void MainWindow::ParametersModify() {
  parameters_dialog_->LoadValues();
  parameters_dialog_->show();
}

void MainWindow::ParametersLoad() {
  auto filename = QFileDialog::getOpenFileName(this, tr("Load parameters"), "",
                                               tr("YAML files (*.yaml)"));
  if (filename.isNull()) {
    return;
  }
  application_->parameters_storage()->LoadFromFile(filename);
}

void MainWindow::ParametersSave() {
  auto filename = QFileDialog::getSaveFileName(this, tr("Save parameters"), "",
                                               tr("YAML files (*.yaml)"));
  if (filename.isNull()) {
    return;
  }
  application_->parameters_storage()->SaveToFile(filename);
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
  ui_->action_calibration_load->setEnabled(mode != Controller::kNonexistent);
  action_group_distortion_->setEnabled(mode != Controller::kNonexistent);

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
  ui_->action_calibration_save->setEnabled(value);

  ui_->action_distortion_data->setEnabled(value);
  ui_->action_distortion_video->setEnabled(value);

  ui_->action_localization_start->setEnabled(value);
}

void MainWindow::SetupStatusBar() {
  controller_status_ = new ControllerStatus();
  statusBar()->addPermanentWidget(controller_status_);

  calibration_status_ = new CalibrationStatus();
  statusBar()->addWidget(calibration_status_);
}

void MainWindow::SetupMenu() {
  action_group_distortion_.reset(new QActionGroup(this));
  ui_->action_distortion_ignore->setActionGroup(action_group_distortion_.get());
  ui_->action_distortion_video->setActionGroup(action_group_distortion_.get());
  ui_->action_distortion_data->setActionGroup(action_group_distortion_.get());


  /* Menu connections */
  connect(ui_->action_abort_calibration, &QAction::triggered,
          this, &MainWindow::AbortCalibration);
  connect(ui_->action_calibrate, &QAction::triggered,
          this, &MainWindow::Calibrate);
  connect(ui_->action_calibration_load, &QAction::triggered,
          this, &MainWindow::CalibrationLoad);
  connect(ui_->action_calibration_save, &QAction::triggered,
          this, &MainWindow::CalibrationSave);
  connect(ui_->action_parameters_modify, &QAction::triggered,
          this, &MainWindow::ParametersModify);
  connect(ui_->action_parameters_load, &QAction::triggered,
          this, &MainWindow::ParametersLoad);
  connect(ui_->action_parameters_save, &QAction::triggered,
          this, &MainWindow::ParametersSave);
  connect(ui_->action_setup_cameras, &QAction::triggered,
          this, &MainWindow::SetupCameras);
  connect(action_group_distortion_.get(), &QActionGroup::triggered,
          this, &MainWindow::GroupDistortion);

}
} // end namespace gui
