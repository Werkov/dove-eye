#include <vector>
#include <string>

#include <QApplication>
#include <QThread>

#include "dove_eye/file_video_provider.h"
#include "dove_eye/frameset_aggregator.h"
#include "main_window.h"
#include "frameset_converter.h"
#include "frameset_provider.h"
#include "frameset_viewer.h"

using dove_eye::FileVideoProvider;
using dove_eye::VideoProvider;
using gui::FramesetConverter;
using gui::FramesetProvider;
using gui::FramesetViewer;
using std::string;
using std::vector;

typedef FramesetProvider::InnerFrameProvider InnerFrameProvider;
Q_DECLARE_METATYPE(InnerFrameProvider *);

int main(int argc, char* argv[]) {
  qRegisterMetaType<InnerFrameProvider *>();
  qRegisterMetaType<gui::FramesetConverter::ImageList>();
  qRegisterMetaType<gui::FramesetConverter::ImageList>("ImageList");
  qRegisterMetaType<dove_eye::Frameset>();

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

  FramesetProvider provider(aggregator.width());
  FramesetConverter converter(aggregator.width());

  MainWindow main_window;

  auto viewer = FramesetViewer::createWithLayout(aggregator.width(),
                                                 new QHBoxLayout(),
                                                 &main_window);

  QThread provider_thread, converter_thread;

  provider_thread.start();
  converter_thread.start();

  provider.moveToThread(&provider_thread);
  converter.moveToThread(&converter_thread);

  QObject::connect(&provider, &FramesetProvider::FramesetReady,
                   &converter, &FramesetConverter::ProcessFrameset);
  QObject::connect(&converter, &FramesetConverter::ImagesetReady,
                   viewer, &FramesetViewer::SetImageset);

  main_window.show();


  QMetaObject::invokeMethod(&provider, "Start",
                            Q_ARG(InnerFrameProvider *, &aggregator));

  int rc = app.exec();

  provider_thread.quit();
  converter_thread.quit();
  provider_thread.wait();
  converter_thread.wait();
  return rc;
}

