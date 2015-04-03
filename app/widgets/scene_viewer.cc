#include "widgets/scene_viewer.h"

#include <cassert>
#include <iostream>

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
    // FIXME use some cyclic/limited buffer
    trajectory_.push_back(location_);
  }
}

void SceneViewer::SetDrawTrajectory(const bool value) {
  if (!value) {
    trajectory_.clear();
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
    cameras_[cam] = std::move(CameraPtr(camera));
  }

  /* First camera position is given directly */
  cv::Mat r0 = data.rotation().inv();
  cv::Mat t0 = -data.rotation() * data.position();

  PositionCamera(r0, t0, cameras_[0].get());

  /* Other cameras are taken relatively to the first (0-th) camera. */
  for (CameraIndex cam = 1; cam < data.Arity(); ++cam) {
    auto &pair = data.pair_parameters(cam - 1);

    cv::Mat r = pair.rotation * r0;
    cv::Mat t = pair.rotation * t0 + pair.translation;

    PositionCamera(r, t, cameras_[cam].get());
  }
}

/** Put camera to position specified by r and t matrices
 *
 * x_camera = r * x_world + t
 *
 * @param[in]  r  world-to-camera rotation
 * @param[in]  t  world-to-camera translation
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
