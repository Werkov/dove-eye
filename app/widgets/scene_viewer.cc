#include "widgets/scene_viewer.h"

#include <cassert>
#include <iostream>
#include <limits>

#include <opencv2/opencv.hpp>

#include "dove_eye/types.h"

using dove_eye::CameraIndex;
using dove_eye::Location;
using qglviewer::Camera;
using qglviewer::Quaternion;
using qglviewer::Vec;

namespace widgets {

void SceneViewer::SetLocation(const dove_eye::Location &location) {
  location_ = location;

  if (draw_trajectory_) {
    TrajectoryAppend(location_);
  }
}

void SceneViewer::SetDrawTrajectory(const bool value) {
  if (!value) {
    TrajectoryClear();
  }
}

void SceneViewer::SetCalibrationData(const dove_eye::CalibrationData &data) {
  CreateCameras(data);
  // TODO something else?
}

void SceneViewer::init() {
  setGridIsDrawn();
  setAxisIsDrawn();
  startAnimation();
  SetDrawTrajectory();
}

void SceneViewer::draw() {
  if (draw_trajectory_) {
    glLineWidth(1.0);
    glBegin(GL_LINE_STRIP);
      glColor3f(0.5, 0.5, 0.5);
      for (auto location : trajectory_) {
        glVertex3f(location.x, location.y, location.z);
      }
    glEnd();
  }

  glPointSize(10.0f);
  glDisable(GL_LIGHTING);
  glBegin(GL_POINTS);
    glColor3f(1.0, 0.2f, 0);
    glVertex3f(location_.x, location_.y, location_.z);
  glEnd();

  /* Draw cameras */
  glLineWidth(4.0);
  glColor4f(1.0, 1.0, 1.0, 0.5);
  for (auto &camera : cameras_) {
    camera->draw();
  }
  glEnable(GL_LIGHTING);
}


void SceneViewer::CreateCameras(const dove_eye::CalibrationData &data) {
  cameras_.resize(data.Arity());

  for (CameraIndex cam = 0; cam < data.Arity(); ++cam) {
    auto camera = new Camera();

    auto rot = data.CameraRotation(cam);
    auto trans = data.CameraTranslation(cam);
    PositionCamera(rot, trans, camera);

    cameras_[cam] = std::move(CameraPtr(camera));
  }
}

void SceneViewer::TrajectoryClear() {
  trajectory_.clear();
  auto inf = std::numeric_limits<double>::infinity();
  trajectory_min_ = Vec(inf, inf, inf);
  trajectory_max_ = Vec(+inf, +inf, +inf);
}

void SceneViewer::TrajectoryAppend(const dove_eye::Location &location) {
  using std::min;
  using std::max;

  // FIXME use some cyclic/limited buffer
  trajectory_.push_back(location);

  trajectory_min_.x = min(static_cast<qreal>(location.x), trajectory_min_.x);
  trajectory_min_.y = min(static_cast<qreal>(location.y), trajectory_min_.y);
  trajectory_min_.z = min(static_cast<qreal>(location.z), trajectory_min_.z);
  
  trajectory_max_.x = max(static_cast<qreal>(location.x), trajectory_max_.x);
  trajectory_max_.y = max(static_cast<qreal>(location.y), trajectory_max_.y);
  trajectory_max_.z = max(static_cast<qreal>(location.z), trajectory_max_.z);

  setSceneBoundingBox(trajectory_min_, trajectory_max_);
}

/** Put camera to position specified by r and t matrices
 *
 *
 * @param[in]  r  world-to-camera rotation
 * @param[in]  t  world-to-camera translation
 *
 * Meaning
 *     x_camera = r * x_world + t
 */
void SceneViewer::PositionCamera(const cv::Mat &r, const cv::Mat &t,
                                 qglviewer::Camera *camera) {
  /*
   * Calculate camera-to-world inverse and project (0, 0, 0)_camera to be
   * position_world of camera.  Similarly project (0, 0, 1)_camera to be view
   * direction (world) and (0, -1, 0) to be up vector (world).
   *
   * There is -1 at y as QGLViewer obviously use opposite y direction.
   */
  const cv::Mat inv_r = r.inv();
  const cv::Mat inv_t = -inv_r * t;

  const cv::Vec3d position(inv_t);
  camera->setPosition(Vec(position));

  const cv::Vec3d z_image(inv_r.col(2));
  camera->setViewDirection(Vec(z_image));

  const cv::Vec3d y_image(inv_r.col(1));
  camera->setUpVector(Vec(-y_image));
}

} // end namespace widgets
