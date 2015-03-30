#include "application.h"

#include "metatypes.h"

using dove_eye::CameraIndex;
using dove_eye::Localization;
using dove_eye::TemplateTracker;
using dove_eye::Tracker;

Application::Application()
    : QObject() {
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


void Application::SetArity(const CameraIndex arity) {
  arity_ = arity;

  SetupController();
  SetupConverter();

  emit ChangedArity(arity);
}

void Application::SetupController() {
  // TODO do not store aggregator (it's not QObject)
//  typedef Controller::InnerFrameProvider Aggregator;
//
//  Aggregator::FramePolicy::ProvidersContainer providers;
//  Aggregator::OffsetsContainer offsets;
//
//  double offs = 0;
//  for (auto filename : args) {
//    providers.push_back(std::unique_ptr<VideoProvider>(
//            new FileVideoProvider(filename)));
//    offsets.push_back(offs);
//    offs += 1.0;
//  }
//  CameraIndex arity = offsets.size();
//
//
//  // TODO window size should be maximum offset
//  Aggregator aggregator(
//      std::move(providers), offsets, 2.0);

  assert(aggregator_);

  TemplateTracker inner_tracker(parameters_);
  auto tracker = new Tracker(arity_, inner_tracker);
  auto localization = new Localization;

  auto new_controller = new Controller(parameters_, aggregator_, tracker,
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

