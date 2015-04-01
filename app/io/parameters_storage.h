#ifndef IO_PARAMETERS_STORAGE_H_
#define IO_PARAMETERS_STORAGE_H_

#include <QObject>
#include <QString>

#include "dove_eye/parameters.h"

namespace io {

class ParametersStorage : public QObject {
  Q_OBJECT
 public:
  explicit ParametersStorage(dove_eye::Parameters & parameters,
                             QWidget *parent = nullptr)
      : parameters_(parameters) {
  }


 public slots:
  void LoadFromFile(const QString &filename);
  void SaveToFile(const QString &filename);

 private:
  dove_eye::Parameters &parameters_;

  QString NormalizeName(const QString &name);
};

} // end namespace io

#endif // IO_PARAMETERS_STORAGE_H_
