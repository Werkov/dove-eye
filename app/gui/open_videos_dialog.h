#ifndef GUI_OPEN_VIDEOS_DIALOG_H_
#define GUI_OPEN_VIDEOS_DIALOG_H_

#include <memory>

#include <QDialog>
#include <QString>

#include "application.h"
#include "dove_eye/file_video_provider.h"
#include "dove_eye/types.h"

namespace Ui {
class OpenVideosDialog;
}

namespace gui {

class OpenVideosDialog : public QDialog {
  Q_OBJECT
 public:
  explicit OpenVideosDialog(QWidget *parent = nullptr);

  ~OpenVideosDialog() override;

  void SetMaxArity(const dove_eye::CameraIndex max_arity);

  void SetProvidersContainer(Application::VideoProvidersVectorOwning *ptr);

 signals:
  void SelectedProviders(const Application::ProvidersType type,
                         const Application::VideoProvidersVector &providers) const;

 private slots:
  void OnAccepted() const;

 private:
  std::unique_ptr<Ui::OpenVideosDialog> ui_;

  dove_eye::FileVideoProvider *CreateVideoProvider(const QString &filename) const;

  Application::VideoProvidersVectorOwning *providers_ptr_;
};

} // end namespace gui

#endif // GUI_OPEN_VIDEOS_DIALOG_H_
