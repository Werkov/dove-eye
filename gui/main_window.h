#ifndef GUI_MAIN_WINDOW_H_
#define GUI_MAIN_WINDOW_H_

#include <memory>

#include <QMainWindow>
#include <QThread>

#include "frameset_converter.h"
#include "frameset_provider.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(gui::FramesetProvider::InnerFrameProvider *aggregator,
                      QWidget *parent = nullptr);

  ~MainWindow() override;

 private:
  std::unique_ptr<Ui::MainWindow> ui_;

  QThread converter_thread_;
  QThread provider_thread_;

  std::unique_ptr<gui::FramesetProvider> provider_;
  std::unique_ptr<gui::FramesetConverter> converter_;
};

#endif // GUI_MAIN_WINDOW_H_
