#include "application.h"

#include <QMetaObject>
#include <QtDebug>

#include "dove_eye/camera_calibration.h"
#include "dove_eye/camera_video_provider.h"
#include "dove_eye/camshift_tracker.h"
#include "dove_eye/chessboard_pattern.h"
#include "dove_eye/histogram_tracker.h"
#include "dove_eye/template_tracker.h"
#include "dove_eye/tracker.h"
#include "metatypes.h"

using dove_eye::CalibrationData;
using dove_eye::CameraCalibration;
using dove_eye::CameraIndex;
using dove_eye::CameraVideoProvider;
using dove_eye::CamshiftTracker;
using dove_eye::ChessboardPattern;
using dove_eye::HistogramTracker;
using dove_eye::Localization;
using dove_eye::Parameters;
using dove_eye::TemplateTracker;
using dove_eye::Tracker;
using std::unique_ptr;

Application::Application()
    : QObject(),
      arity_(0),
      parameters_(),
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

Application::VideoProvidersVector Application::AvailableVideoProviders() {
  if (available_providers_.size() == 0) {
    /* Scan device IDs from 0 to first invalid (with at most skip errors) */
    const int skip = 10;
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
      ++device;
    }
  }

  VideoProvidersVector result;
  for (auto &provider : available_providers_) {
    result.push_back(provider.get());
  }
  return result;
}

void Application::UseCameraProviders(const VideoProvidersVector &providers) {
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
  /*
   * Release unused providers, so that available_providers_ is empty for next
   * discovery.
   */
  available_providers_.clear();

  arity_ = used_providers.size();

  SetupController(std::move(used_providers));
  SetupConverter();

  emit SetupPipeline();

  /* Asynchronously start new controller. */
  QMetaObject::invokeMethod(controller_, "Start");
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

void Application::MoveToNewThread(QObject* object) {
  QThread* thread = new QThread(this);

  MoveToThread(object, thread);

  thread->start();
  threads_ << thread;
}

void Application::MoveToThread(QObject* object, QThread* thread) {
  object->setParent(nullptr);
  //object->moveToThread(thread);
  objects_in_threads_ << object;
}


void Application::SetupController(VideoProvidersContainer &&providers) {
  auto aggregator = new Controller::Aggregator(std::move(providers), parameters_);

  auto pattern = new ChessboardPattern(
      parameters_.Get(Parameters::CALIBRATION_ROWS),
      parameters_.Get(Parameters::CALIBRATION_COLS),
      parameters_.Get(Parameters::CALIBRATION_SIZE));
  auto calibration = new CameraCalibration(parameters_, arity_, pattern);

  //TemplateTracker inner_tracker(parameters_);
  //CamshiftTracker inner_tracker(parameters_);
  HistogramTracker inner_tracker(parameters_);
  auto tracker = new Tracker(arity_, inner_tracker);
  auto localization = new Localization(arity_);

  auto new_controller = new Controller(parameters_, aggregator, calibration,
                                       tracker, localization);
  //new_controller->SetTrackerMarkType(Controller::kCircle);
  new_controller->SetTrackerMarkType(Controller::kRectangle);

  connect(new_controller, &Controller::CalibrationDataReady,
          this, &Application::SetCalibrationData);
  connect(this, &Application::CalibrationDataReady,
          new_controller, &Controller::SetCalibrationData);

  SwapAndDestroy(&controller_, new_controller);
}

void Application::SetupConverter() {
  assert(controller_);

  auto new_converter = new FramesetConverter(arity_);
  SwapAndDestroy(&converter_, new_converter);

  QObject::connect(controller_, &Controller::FramesetReady,
                   converter_, &FramesetConverter::ProcessFrameset);
  QObject::connect(converter_, &FramesetConverter::MarkCreated,
                   controller_, &Controller::SetMark);

}

