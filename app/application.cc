#include "application.h"

#include "metatypes.h"

using dove_eye::CameraIndex;
using dove_eye::Localization;
using dove_eye::TemplateTracker;
using dove_eye::Tracker;

Application::Application()
    : QObject(),
      arity_(0),
      controller_(nullptr),
      converter_(nullptr) {
  RegisterMetaTypes();

  //TODO call SetArity (together with providers API)
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
    // TODO discover available video providers
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
    for (auto &provider_owner : available_providers_) {
      bool released = false;
      if (provider_owner.get() == provider) {
        provider_owner.release();
        released = true;
        break;
      }
      assert(released);
    }
    used_providers.push_back(
        std::move(VideoProvidersContainer::value_type(provider)));
  }

  arity_ = used_providers.size();

  SetupController(std::move(used_providers));
  SetupConverter();

  emit ChangedArity(arity_);
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
  //TODO replace deprecated offsets API
  Controller::Aggregator::OffsetsContainer offsets(providers.size(), 0);
  auto aggregator = new Controller::Aggregator(std::move(providers), offsets, 0.1);

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

