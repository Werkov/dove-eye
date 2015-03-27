#include <vector>
#include <string>

#include <QApplication>

#include "controller.h"
#include "dove_eye/file_video_provider.h"
#include "dove_eye/frameset_aggregator.h"
#include "dove_eye/localization.h"
#include "dove_eye/parameters.h"
#include "dove_eye/template_tracker.h"
#include "dove_eye/tracker.h"
#include "dove_eye/types.h"
#include "main_window.h"

using dove_eye::CameraIndex;
using dove_eye::Parameters;
using dove_eye::FileVideoProvider;
using dove_eye::TemplateTracker;
using dove_eye::VideoProvider;
using std::string;
using std::vector;

int main(int argc, char* argv[]) {

  vector<string> args(argv + 1, argv + argc);

  QApplication app(argc, argv);

  typedef gui::Controller::InnerFrameProvider Aggregator;

  Aggregator::FramePolicy::ProvidersContainer providers;
  Aggregator::OffsetsContainer offsets;

  double offs = 0;
  for (auto filename : args) {
    providers.push_back(std::unique_ptr<VideoProvider>(
            new FileVideoProvider(filename)));
    offsets.push_back(offs);
    offs += 1.0;
  }
  CameraIndex arity = offsets.size();


  // TODO window size should be maximum offset
  Aggregator aggregator(
      std::move(providers), offsets, 2.0);



  Parameters parameters;
  dove_eye::Tracker tracker(arity, TemplateTracker(parameters));
  dove_eye::Localization localization;
  gui::Controller controller(parameters, aggregator, tracker, localization);

  MainWindow main_window(&controller);
  main_window.show();


  int rc = app.exec();

  // TODO threads in window?
  return rc;
}

