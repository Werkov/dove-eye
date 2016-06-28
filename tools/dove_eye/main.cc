#include <vector>
#include <string>

#include <QApplication>

#include "application.h"
#include "dove_eye/types.h"
#include "gui/main_window.h"

using dove_eye::CameraIndex;
using gui::MainWindow;
using std::string;
using std::vector;

int main(int argc, char* argv[]) {
  vector<string> args(argv + 1, argv + argc);

  QApplication app(argc, argv);

  Application application;

  MainWindow main_window(&application);
  main_window.show();

  int rc = app.exec();

  return rc;
}

