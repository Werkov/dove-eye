#ifndef GUI_MAIN_WINDOW_H_
#define GUI_MAIN_WINDOW_H_

#include <memory>

#include <QActionGroup>
#include <QMainWindow>
#include <QThread>

#include "application.h"
#include "dove_eye/calibration_data.h"
#include "dove_eye/types.h"
#include "parameters_dialog.h"
#include "cameras_setup_dialog.h"
#include "widgets/calibration_status.h"
#include "widgets/controller_status.h"

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
  void SetUndistortMode(const Controller::UndistortMode undistort_mode);

 public slots:
  void SetupPipeline();
  void CalibrationDataReady(const dove_eye::CalibrationData data);

 private slots:
  void AbortCalibration();
  void Calibrate();
  void CalibrationLoad();
  void CalibrationSave();
  void GroupDistortion(QAction *action);
  void ParametersModify();
  void ParametersLoad();
  void ParametersSave();
  void SetupCameras();
  void ControllerModeChanged(const Controller::Mode mode);
  void SetCalibration(const bool value);

 private:
  Application *application_;

  std::unique_ptr<Ui::MainWindow> ui_;

  // FIXME consider using parenting to window instead of unique_ptr (could the
  //       inline dtor)
  std::unique_ptr<QActionGroup> action_group_distortion_;
  std::unique_ptr<ParametersDialog> parameters_dialog_;
  std::unique_ptr<CamerasSetupDialog> cameras_setup_dialog_;

  widgets::ControllerStatus *controller_status_;
  widgets::CalibrationStatus *calibration_status_;

  void SetupStatusBar();
  void SetupMenu();
};

} // end namespace gui

#endif // GUI_MAIN_WINDOW_H_
