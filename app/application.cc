#include "application.h"

#include <QMetaObject>
#include <QtDebug>

#include "dove_eye/aggregator.h"
#include "dove_eye/async_policy.h"
#include "dove_eye/blocking_policy.h"
#include "dove_eye/camera_calibration.h"
#include "dove_eye/camera_video_provider.h"
#include "dove_eye/chessboard_pattern.h"
#include "dove_eye/circle_tracker.h"
#include "dove_eye/frameset.h"
#include "dove_eye/frameset_aggregator.h"
#include "dove_eye/histogram_tracker.h"
#include "dove_eye/template_tracker.h"
#include "dove_eye/tld_tracker.h"
#include "dove_eye/tracker.h"
#include "metatypes.h"


using dove_eye::Aggregator;
using dove_eye::AsyncPolicy;
using dove_eye::BlockingPolicy;
using dove_eye::CalibrationData;
using dove_eye::CameraCalibration;
using dove_eye::CameraIndex;
using dove_eye::CameraVideoProvider;
using dove_eye::ChessboardPattern;
using dove_eye::CircleTracker;
using dove_eye::Frameset;
using dove_eye::HistogramTracker;
using dove_eye::Localization;
using dove_eye::Parameters;
using dove_eye::TemplateTracker;
using dove_eye::TldTracker;
using dove_eye::Tracker;
using std::unique_ptr;

Application::Application()
    : QObject(),
      arity_(0),
      parameters_(),
      available_trackers_({
        {kTrackerCircle, "CircleTracker"},
        {kTrackerHistogram, "HistogramTracker"},
        {kTrackerTemplate, "TemplateTracker"},
        {kTrackerTld, "TldTracker"}
      }),
      parameters_storage_(parameters_),
      controller_(nullptr),
      converter_(nullptr) {
  RegisterMetaTypes();
}

Application::~Application() {
  for (auto object : objects_in_threads_) {
    object->deleteLater();
  }

  for (auto thread : threads_) {
    thread->quit();
  }

  for (auto thread : threads_) {
    thread->wait();
  }
}

const Application::TrackerNamesMap &Application::available_trackers() const {
  return available_trackers_;
}

Application::VideoProvidersVector Application::ScanCameraProviders() {
  InitializeEmpty();

  assert(available_providers_.size() == 0);

  /* Scan device IDs from 0 to first invalid (with at most skip errors) */
  const int skip = Frameset::kMaxArity;
  const int tests = 2 * Frameset::kMaxArity;
  int device = 0;
  int errors = 0;
  while (true) {
    auto provider = new CameraVideoProvider(device);
    if (provider->begin() != provider->end()) {
      available_providers_.push_back(
          VideoProvidersVectorOwning::value_type(provider));
      qDebug() << "Found working camera device" << device;
    } else {
      delete provider;
      qDebug() << "Camera device" << device << "not working";
      if (++errors >= skip) {
        break;
      }
    }
    if (++device >= tests) {
      break;
    }
  }

  VideoProvidersVector result;
  for (auto &provider : available_providers_) {
    result.push_back(provider.get());
  }
  return result;
}

void Application::ReleaseAvailableProviders() {
  InitializeEmpty();
  available_providers_.clear();
}

void Application::InitializeEmpty() {
  providers_type_ = kNoProviders;
  available_providers_.clear();

  arity_ = 0;
  TeardownConverter();
  TeardownController();

  emit SetupPipeline();
}

void Application::Initialize(const ProvidersType type,
                             const VideoProvidersVector &providers) {
  providers_type_ = type;

  /*
   * We should obtain a subset of providers that exist in available_providers_.
   * Move ownership of chosen providers from application to controller and
   * dispose remaining providers (by clearing available_providers_).
   *
   */
  VideoProvidersContainer used_providers;
  for (auto provider : providers) {
    bool released = false;
    for (auto &provider_owner : available_providers_) {
      if (provider_owner.get() == provider) {
        provider_owner.release();
        released = true;
        break;
      }
    }
    assert(released);
    used_providers.push_back(
        std::move(VideoProvidersContainer::value_type(provider)));
  }
  available_providers_.clear();

  /* Setup components */
  arity_ = used_providers.size();

  SetupController(type, std::move(used_providers));
  SetTrackerType(kTrackerCircle);
  SetupConverter();

  emit SetupPipeline();

  /* Asynchronously start new controller. */
  bool paused = (type != kCameras);
  QMetaObject::invokeMethod(controller_, "Start",
                            Q_ARG(bool, paused));
}

void Application::SetCalibrationData(const CalibrationData calibration_data) {
  assert(controller_);

  // TODO this should be displayed as a user error
  assert(controller_->Arity() == calibration_data.Arity());

  calibration_data_ = std::move(unique_ptr<CalibrationData>(
          new CalibrationData(calibration_data)));
  /*
   * Application is primary holder of calibration data, thus we signal each
   * change in it to all slots.
   */
  emit CalibrationDataReady(calibration_data);
}

void Application::SetTrackerType(const InnerTrackerType type) {
  unique_ptr<dove_eye::InnerTracker> inner_tracker;
  switch(type) {
  case kTrackerCircle:
      inner_tracker.reset(new CircleTracker(parameters_));
      break;
  case kTrackerHistogram:
      inner_tracker.reset(new HistogramTracker(parameters_));
      break;
  case kTrackerTemplate:
      inner_tracker.reset(new TemplateTracker(parameters_));
      break;
  case kTrackerTld:
      inner_tracker.reset(new TldTracker(parameters_));
      break;
  }

  /* Signal receiver will take ownership of the created Tracker */
  auto tracker = new Tracker(arity_, *inner_tracker);
  emit ChangedTracker(tracker);
}

void Application::MoveToNewThread(QObject* object) {
#ifndef CONFIG_SINGLE_THREADED
  QThread* thread = new QThread(this);

  MoveToThread(object, thread);

  thread->start();
  threads_ << thread;
#endif
}

void Application::MoveToThread(QObject* object, QThread* thread) {
#ifndef CONFIG_SINGLE_THREADED
  object->setParent(nullptr);
  object->moveToThread(thread);
  objects_in_threads_ << object;
#endif
}

void Application::SetupController(const ProvidersType type,
                                  VideoProvidersContainer &&providers) {
  assert(providers.size() > 0);

  Aggregator *aggregator = nullptr;
  switch (type) {
    case kCameras:
      aggregator = new dove_eye::FramesetAggregator<AsyncPolicy<true>>(
          std::move(providers), parameters_);
      break;
    case kVideoFiles:
      aggregator = new dove_eye::FramesetAggregator<BlockingPolicy>(
          std::move(providers), parameters_);
      break;
    case kNoProviders:
      assert(false);
      break;
  }


  auto pattern = new ChessboardPattern(
      parameters_.Get(Parameters::CALIBRATION_ROWS),
      parameters_.Get(Parameters::CALIBRATION_COLS),
      parameters_.Get(Parameters::CALIBRATION_SIZE));
  auto calibration = new CameraCalibration(parameters_, arity_, pattern);

  auto localization = new Localization(arity_);

  auto new_controller = new Controller(parameters_, aggregator, calibration,
                                       localization);

  connect(new_controller, &Controller::CalibrationDataReady,
          this, &Application::SetCalibrationData);
  connect(this, &Application::CalibrationDataReady,
          new_controller, &Controller::SetCalibrationData);
  connect(this, &Application::ChangedTracker,
          new_controller, &Controller::SetTracker);

  SwapAndDestroy(&controller_, new_controller);
}

void Application::TeardownController() {
  SwapAndDestroy(&controller_, static_cast<Controller *>(nullptr), true);
}

void Application::SetupConverter() {
  assert(controller_);

  auto new_converter = new FramesetConverter(arity_);
  SwapAndDestroy(&converter_, new_converter);

  QObject::connect(controller_, &Controller::FramesetReady,
                   converter_, &FramesetConverter::ProcessFrameset);
  QObject::connect(controller_, &Controller::PositsetReady,
                   converter_, &FramesetConverter::ProcessPositset);
  QObject::connect(converter_, &FramesetConverter::MarkCreated,
                   controller_, &Controller::SetMark);

}

void Application::TeardownConverter() {
  SwapAndDestroy(&converter_, static_cast<FramesetConverter *>(nullptr), true);
}



