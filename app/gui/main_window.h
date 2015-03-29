#ifndef GUI_MAIN_WINDOW_H_
#define GUI_MAIN_WINDOW_H_

#include <memory>

#include <QMainWindow>
#include <QThread>

#include "controller.h"
#include "dove_eye/parameters.h"
#include "frameset_converter.h"
#include "parameters_dialog.h"
#include "video_providers_dialog.h"

namespace Ui {
class MainWindow;
}

namespace gui {

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(dove_eye::Parameters &parameters,
                      Controller *controller,
                      QWidget *parent = nullptr);

  ~MainWindow() override;

 private slots:
  void ModifyParameters();

 private:
  std::unique_ptr<Ui::MainWindow> ui_;

  QThread controller_thread_;
  QThread converter_thread_;

  std::unique_ptr<FramesetConverter> converter_;

  std::unique_ptr<ParametersDialog> parameters_dialog_;
  std::unique_ptr<VideoProvidersDialog> video_providers_dialog_;
};

} // end namespace gui

#endif // GUI_MAIN_WINDOW_H_
