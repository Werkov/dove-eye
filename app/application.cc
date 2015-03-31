#include "application.h"

#include <QMetaObject>
#include <QtDebug>

#include "dove_eye/camera_video_provider.h"
#include "metatypes.h"

using dove_eye::CameraIndex;
using dove_eye::CameraVideoProvider;
using dove_eye::Localization;
using dove_eye::TemplateTracker;
using dove_eye::Tracker;

Application::Application()
    : QObject(),
      arity_(0),
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
    /* Scan device IDs from 0 to first invalid */
    int device = 0;
    while (true) {
      auto provider = new CameraVideoProvider(device);
      if (provider->begin() != provider->end()) {
        available_providers_.push_back(
            VideoProvidersVectorOwning::value_type(provider));
        qDebug() << "Found working camera device" << device;
      } else {
        delete provider;
        qDebug() << "Camera device" << device << "not working";
        break;
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

void Application::UseVideoProviders(const VideoProvidersVector &providers) {
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

  emit ChangedArity(arity_);

  /* Asynchronously start new controller. */
  QMetaObject::invokeMethod(controller_, "Start");
}

void Application::MoveToNewThread(QObject* object) {
  QThread* thread = new QThread(this);

  MoveToThread(object, thread);

  thread->start();
  threads_ << thread;
}

void Application::MoveToThread(QObject* object, QThread* thread) {
  object->setParent(nullptr);
  object->moveToThread(thread);
  objects_in_threads_ << object;
}


void Application::SetupController(VideoProvidersContainer &&providers) {
  auto aggregator = new Controller::Aggregator(std::move(providers), parameters_);

  TemplateTracker inner_tracker(parameters_);
  auto tracker = new Tracker(arity_, inner_tracker);
  auto localization = new Localization;

  auto new_controller = new Controller(parameters_, aggregator, tracker,
                                       localization);

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

