#include "io/calibration_data_storage.h"

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_data.h"
#include "dove_eye/camera_pair.h"
#include "dove_eye/types.h"

using cv::FileStorage;
using cv::FileNode;
using dove_eye::CalibrationData;
using dove_eye::CameraIndex;
using dove_eye::CameraPair;

namespace io {

CalibrationData CalibrationDataStorage::LoadFromFile(const QString &filename) {
  FileStorage fs(filename.toStdString(), FileStorage::READ);

  CameraIndex arity;
  fs[kNameArity] >> arity;
  CameraIndex pairity = CameraPair::Pairity(arity);
  CalibrationData result(arity);

  cv::Mat tmp;
  fs[kNamePosition] >> tmp;
  result.position(tmp);

  fs[kNameRotation] >> tmp;
  result.rotation(tmp);

  auto node = fs[kNameCameraMatrix];
  assert(node.type() == FileNode::SEQ);

  CameraIndex cam = 0;
  for (auto file_node : node) {
    assert(cam < arity);
    file_node >> result.camera_parameters_[cam].camera_matrix;
    ++cam;
  }

  node = fs[kNameDistortionCoefficients];
  cam = 0;
  for (auto file_node : node) {
    assert(cam < arity);
    file_node >> result.camera_parameters_[cam].distortion_coefficients;
    ++cam;
  }

  node = fs[kNameFundamentalMatrix];
  CameraIndex index = 0;
  for (auto file_node : node) {
    assert(index < pairity);
    file_node >> result.pair_parameters_[index].fundamental_matrix;
    ++index;
  }
  
  node = fs[kNamePairRotation];
  index = 0;
  for (auto file_node : node) {
    assert(index < pairity);
    file_node >> result.pair_parameters_[index].rotation;
    ++index;
  }

  node = fs[kNameTranslation];
  index = 0;
  for (auto file_node : node) {
    assert(index < pairity);
    file_node >> result.pair_parameters_[index].translation;
    ++index;
  }

  return result;
}

void CalibrationDataStorage::SaveToFile(const QString &filename,
                                        const CalibrationData &data) {
  FileStorage fs(filename.toStdString(), FileStorage::WRITE);

  fs << kNameArity << data.Arity();
  fs << kNamePosition << data.position();
  fs << kNameRotation << data.rotation();

  fs << kNameCameraMatrix << "[";
  for (CameraIndex cam = 0; cam < data.Arity(); ++cam) {
    fs << data.camera_parameters(cam).camera_matrix;
  }
  fs << "]";

  fs << kNameDistortionCoefficients << "[";
  for (CameraIndex cam = 0; cam < data.Arity(); ++cam) {
    fs << data.camera_parameters(cam).distortion_coefficients;
  }
  fs << "]";

  fs << kNameFundamentalMatrix << "[";
  for (CameraIndex index = 0; index < CameraPair::Pairity(data.Arity()); ++index) {
    fs << data.pair_parameters(index).fundamental_matrix;
  }
  fs << "]";

  fs << kNamePairRotation << "[";
  for (CameraIndex index = 0; index < CameraPair::Pairity(data.Arity()); ++index) {
    fs << data.pair_parameters(index).rotation;
  }
  fs << "]";

  fs << kNameTranslation << "[";
  for (CameraIndex index = 0; index < CameraPair::Pairity(data.Arity()); ++index) {
    fs << data.pair_parameters(index).translation;
  }
  fs << "]";
}

} // end namespace io
