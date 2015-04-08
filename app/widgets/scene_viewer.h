#ifndef WIDGETS_SCENE_VIEWER_H_
#define WIDGETS_SCENE_VIEWER_H_

#include <memory>
#include <vector>

#include <QGLViewer/qglviewer.h>

#include "dove_eye/calibration_data.h"
#include "dove_eye/location.h"

namespace widgets {

class SceneViewer : public QGLViewer {
  Q_OBJECT
 public:
  explicit SceneViewer(QWidget *parent = nullptr)
      : QGLViewer(parent),
        draw_cameras_(false) {
  }

 public slots:
  void SetLocation(const dove_eye::Location &location);

  void SetDrawTrajectory(const bool value = true);
  void SetDrawCameras(const bool value = true);

  void SetCalibrationData(const dove_eye::CalibrationData &data);

  void TrajectoryClear();

 protected:
  void init() override;
  void draw() override;

 private:
  typedef std::unique_ptr<qglviewer::Camera> CameraPtr;
  typedef std::vector<CameraPtr> CamerasVector;

  bool draw_trajectory_;
  bool draw_cameras_;

  QVector<dove_eye::Location> trajectory_;
  qglviewer::Vec trajectory_min_;
  qglviewer::Vec trajectory_max_;

  bool has_location_;
  dove_eye::Location location_;

  CamerasVector cameras_;

  void CreateCameras(const dove_eye::CalibrationData &data);


  void TrajectoryAppend(const dove_eye::Location &location);

  static void PositionCamera(const cv::Mat &r, const cv::Mat &t,
                             qglviewer::Camera *camera);

  static inline qglviewer::Vec VecMin(const qglviewer::Vec &a,
                                      const qglviewer::Vec &b) {
    using std::min;
    qglviewer::Vec result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    return result;
  }

  static inline qglviewer::Vec VecMax(const qglviewer::Vec &a,
                                      const qglviewer::Vec &b) {
    using std::max;
    qglviewer::Vec result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    return result;
  }

};

} // end namespace widgets

#endif // WIDGETS_SCENE_VIEWER_H_
