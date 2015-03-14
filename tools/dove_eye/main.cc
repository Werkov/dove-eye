#include <vector>
#include <string>

#include <QApplication>

#include "dove_eye/file_video_provider.h"
#include "dove_eye/frameset_aggregator.h"
#include "frameset_provider.h"
#include "main_window.h"

using dove_eye::FileVideoProvider;
using dove_eye::VideoProvider;
using std::string;
using std::vector;

int main(int argc, char* argv[]) {

  vector<string> args(argv + 1, argv + argc);

  QApplication app(argc, argv);

  typedef gui::FramesetProvider::InnerFrameProvider Aggregator;

  Aggregator::FramePolicy::ProvidersContainer providers;
  Aggregator::OffsetsContainer offsets;

  double offs = 0;
  for (auto filename : args) {
    providers.push_back(std::unique_ptr<VideoProvider>(
            new FileVideoProvider(filename)));
    offsets.push_back(offs);
    offs += 1.0;
  }


  // TODO window size should be maximum offset
  Aggregator aggregator(
      std::move(providers), offsets, 2.0);


  MainWindow main_window(&aggregator);
  main_window.show();


  int rc = app.exec();

  // TODO threads in window?
  return rc;
}

