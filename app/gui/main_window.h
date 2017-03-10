#ifndef GUI_MAIN_WINDOW_H_
#define GUI_MAIN_WINDOW_H_

#include <memory>

#include <QActionGroup>
#include <QMainWindow>
#include <QThread>

#include "application.h"
#include "cameras_setup_dialog.h"
#include "dove_eye/calibration_data.h"
#include "dove_eye/types.h"
#include "open_videos_dialog.h"
#include "parameters_dialog.h"
#include "widgets/calibration_status.h"
#include "widgets/controller_status.h"
#include "widgets/playback_control.h"

namespace Ui {
class MainWindow;
}

namespace gui {

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(Application *application, QWidget *parent = nullptr);

  ~MainWindow() override;

 signals:
  void SetControllerMode(const Controller::Mode mode);
  void SetLocalizationActive(const bool value);
  void SetUndistortMode(const Controller::UndistortMode undistort_mode);
  void SetTrackerType(const Application::InnerTrackerType type);

 public slots:
  void SetupPipeline();
  void CalibrationDataReady(const dove_eye::CalibrationData data);

 private slots:
  void AbortCalibration();
  void Calibrate();
  void CalibrationLoad();
  void CalibrationSave();
  void LocalizationStart();
  void LocalizationStop();
  void LocalizationSave();
  void GroupDistortion(QAction *action);
  void GroupTrackers(QAction *action);
  void SceneShowCameras();
  void SceneClearTrajectory();
  void OpenVideoFiles();
  void ParametersModify();
  void ParametersLoad();
  void ParametersSave();
  void SetupCameras();
  void SetTracker();
  void ControllerModeChanged(const Controller::Mode mode);
  void SetCalibration(const bool value);

 private:
  Application *application_;

  std::unique_ptr<Ui::MainWindow> ui_;

  QActionGroup *action_group_distortion_;
  ParametersDialog *parameters_dialog_;
  CamerasSetupDialog *cameras_setup_dialog_;
  OpenVideosDialog *open_videos_dialog_;
  QActionGroup *action_group_trackers_;
  QAction *action_default_tracker_;

  widgets::ControllerStatus *controller_status_;
  widgets::CalibrationStatus *calibration_status_;

  void SetupStatusBar();
  void SetupMenu();
  void SetupTrackingMenu();
};

} // end namespace gui

#endif // GUI_MAIN_WINDOW_H_
