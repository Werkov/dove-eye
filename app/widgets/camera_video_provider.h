#ifndef WIDGETS_CAMERA_VIDEO_PROVIDER_H_
#define WIDGETS_CAMERA_VIDEO_PROVIDER_H_

#include <memory>

#include <QWidget>

#include "dove_eye/types.h"
#include "dove_eye/camera_video_provider.h"


namespace Ui {
class CameraVideoProvider;
}

namespace widgets {

class CameraVideoProvider : public QWidget {
  Q_OBJECT
 public:
  explicit CameraVideoProvider(dove_eye::CameraVideoProvider *provider,
                            QWidget *parent = nullptr);

  ~CameraVideoProvider() override;

  bool Selected() const;

  inline dove_eye::CameraVideoProvider *provider() {
    return provider_;
  }

  void ApplySettings();

 private:
  std::unique_ptr<Ui::CameraVideoProvider> ui_;
  dove_eye::CameraVideoProvider *provider_;

  void InitializeUi();
};

} // end namespace widgets

#endif // WIDGETS_CAMERA_VIDEO_PROVIDER_H_
