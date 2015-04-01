#ifndef IO_CALIBRATION_DATA_STORAGE_H_
#define IO_CALIBRATION_DATA_STORAGE_H_

#include <QObject>
#include <QString>

/* Forward */
namespace dove_eye {
class CalibrationData;
}

namespace io {

class CalibrationDataStorage : public QObject {
  Q_OBJECT
 public:
  explicit CalibrationDataStorage(QObject *parent = nullptr)
      : QObject(parent) {
  }


 public slots:
  dove_eye::CalibrationData LoadFromFile(const QString &filename);
  void SaveToFile(const QString &filename,
                  const dove_eye::CalibrationData &data);
 private:
  const char *kNameArity = "arity";
  const char *kNameCameraMatrix = "C";
  const char *kNameDistortionCoefficients = "D";
  const char *kNameEssentialMatrix = "E";
};

} // end namespace io

#endif // IO_CALIBRATION_DATA_STORAGE_H_
