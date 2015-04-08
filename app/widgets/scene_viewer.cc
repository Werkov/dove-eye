#include "widgets/scene_viewer.h"

#include <cassert>
#include <iostream>
#include <limits>

#include <opencv2/opencv.hpp>

#include "dove_eye/types.h"
#include "dove_eye/logging.h"

using dove_eye::CameraIndex;
using dove_eye::Location;
using qglviewer::Camera;
using qglviewer::Quaternion;
using qglviewer::Vec;

namespace widgets {

void SceneViewer::SetLocation(const dove_eye::Location &location) {
  location_ = location;
  has_location_ = true;

  if (draw_trajectory_) {
    TrajectoryAppend(location_);
  }
}

void SceneViewer::SetDrawTrajectory(const bool value) {
  if (!value) {
    TrajectoryClear();
  }
  draw_trajectory_ = value;
}

void SceneViewer::SetDrawCameras(const bool value) {
  draw_cameras_ = value;
}

void SceneViewer::SetCalibrationData(const dove_eye::CalibrationData &data) {
  CreateCameras(data);
  
  /* Same view as 0-th camera, behind it */
  assert(cameras_.size() > 0);
  camera()->setViewDirection(cameras_[0]->viewDirection());
  camera()->setUpVector(cameras_[0]->upVector());
  camera()->setPosition(Vec(0,0,-1));
}

void SceneViewer::init() {
  setGridIsDrawn();
  setAxisIsDrawn();
  startAnimation();
  SetDrawTrajectory();
  has_location_ = false;
}

void SceneViewer::draw() {
  if (draw_trajectory_ && trajectory_.size() > 1) {
    glLineWidth(3.0);
    glBegin(GL_LINE_STRIP);
      glColor3f(0.5, 0.5, 0.5);
      for (auto location : trajectory_) {
        glVertex3f(location.x, location.y, location.z);
      }
    glEnd();
  }

  /* Draw point */
  if (has_location_) {
    glPointSize(10.0f);
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
      glColor3f(1.0, 0.2f, 0);
      glVertex3f(location_.x, location_.y, location_.z);
    glEnd();
    glEnable(GL_LIGHTING);
  }

  /* Draw cameras */
  if (draw_cameras_) {
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);
    glColor4f(1.0, 1.0, 1.0, 0.5);
    for (auto &camera : cameras_) {
      camera->draw();
    }
    glEnable(GL_LIGHTING);
  }
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
  trajectory_max_ = Vec(-inf, -inf, -inf);
  has_location_ = false;
  DEBUG("%s", __func__);
}

void SceneViewer::TrajectoryAppend(const dove_eye::Location &location) {
  using std::min;
  using std::max;

  // FIXME use some cyclic/limited buffer
  trajectory_.push_back(location);

  Vec loc(location.x, location.y, location.z);
  trajectory_min_ = VecMin(loc, trajectory_min_);
  trajectory_max_ = VecMax(loc, trajectory_min_);

  setSceneCenter(Vec(0, 0, 0));
  setSceneRadius(max(trajectory_min_.norm(), trajectory_max_.norm()));
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
