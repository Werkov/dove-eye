#ifndef WIDGETS_VIDEO_PROVIDER_H_
#define WIDGETS_VIDEO_PROVIDER_H_

#include <memory>

#include <QWidget>

#include "dove_eye/types.h"
#include "dove_eye/video_provider.h"


namespace Ui {
class VideoProvider;
}

namespace widgets {

class VideoProvider : public QWidget {
  Q_OBJECT
 public:
  explicit VideoProvider(dove_eye::VideoProvider *provider,
                            QWidget *parent = nullptr);

  ~VideoProvider() override;

  bool Selected() const;

  inline dove_eye::VideoProvider *provider() {
    return provider_;
  }

 private:
  std::unique_ptr<Ui::VideoProvider> ui_;
  dove_eye::VideoProvider *provider_;

};

} // end namespace widgets

#endif // WIDGETS_VIDEO_PROVIDER_H_
