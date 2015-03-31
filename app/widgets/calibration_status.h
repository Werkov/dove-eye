#ifndef WIDGETS_CALIBRATION_STATUS_H_
#define WIDGETS_CALIBRATION_STATUS_H_


#include <QLabel>
#include <QVector>
#include <QWidget>

#include "dove_eye/types.h"


namespace widgets {

class CalibrationStatus : public QWidget {
  Q_OBJECT
 public:
  explicit CalibrationStatus(QWidget *parent = nullptr);

  void SetArity(const dove_eye::CameraIndex arity);

 public slots:
  void CameraCalibrationProgressed(const dove_eye::CameraIndex cam,
                                   const double progress);
  void PairCalibrationProgressed(const dove_eye::CameraIndex index,
                                 const double progress);


 private:
  QVector<double> camera_progress_;
  QVector<double> pair_progress_;

  QLabel *label_;

  void RefreshStatus();
};

} // end namespace widgets

#endif // WIDGETS_CALIBRATION_STATUS_H_
