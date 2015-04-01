#include "io/parameters_storage.h"

#include <opencv2/opencv.hpp>

using cv::FileStorage;
using dove_eye::Parameters;

namespace io {

void ParametersStorage::LoadFromFile(const QString &filename) {
  FileStorage fs(filename.toStdString(), FileStorage::READ);

  for (auto &param : parameters_) {
    double value;
    auto name = NormalizeName(QString::fromStdString(param.name)).toStdString();
    fs[name] >> value;
    parameters_.Set(param.key, value);
  }
}

void ParametersStorage::SaveToFile(const QString &filename) {
  FileStorage fs(filename.toStdString(), FileStorage::WRITE);

  for (auto &param : parameters_) {
    auto name = NormalizeName(QString::fromStdString(param.name)).toStdString();
    fs << name << parameters_.Get(param.key);
  }
}

QString ParametersStorage::NormalizeName(const QString &name) {
  QString result = name;
  result.replace('.', '_')
      .replace('[', "")
      .replace(']', "");
  return result;
}
} // end namespace io
