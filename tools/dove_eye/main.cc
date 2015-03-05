#include <vector>
#include <string>

#include <QApplication>

#include "dove_eye/file_video_provider.h"
#include "dove_eye/frameset_aggregator.h"
#include "main_window.h"

using dove_eye::FileVideoProvider;
using dove_eye::VideoProvider;
using std::string;
using std::vector;

int main(int argc, char* argv[]) {

  vector<string> args(argv + 1, argv + argc);

  QApplication app(argc, argv);

  typedef dove_eye::FramesetAggregator<dove_eye::BlockingPolicy> Aggregator;

  dove_eye::BlockingPolicy::ProvidersContainer providers;
  Aggregator::OffsetsContainer offsets;

  for (auto filename : args) {
    providers.push_back(std::unique_ptr<VideoProvider>(
            new FileVideoProvider(filename)));
    offsets.push_back(0);
  }


  // TODO window size should maximum offset
  Aggregator aggregator(
      std::move(providers), offsets, 2.5);


  MainWindow main_window(&aggregator);
  main_window.show();


  int rc = app.exec();

  // TODO threads in window?
  return rc;
}

