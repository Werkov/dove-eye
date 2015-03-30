#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>
#include <vector>

#include <QList>
#include <QObject>
#include <QThread>

#include "controller.h"
#include "dove_eye/parameters.h"
#include "dove_eye/localization.h"
#include "dove_eye/template_tracker.h"
#include "dove_eye/tracker.h"
#include "dove_eye/types.h"
#include "dove_eye/video_provider.h"
#include "frameset_converter.h"

/** Holds and manages all necessary object of application
 *
 * @note Thread and QObject handling copied from Clementine.
 */
class Application : public QObject {
  Q_OBJECT
 public:
  typedef std::vector<dove_eye::VideoProvider *> VideoProvidersVector;
  typedef std::vector<std::unique_ptr<dove_eye::VideoProvider>>
      VideoProvidersVectorOwning;
  typedef Controller::Aggregator::ProvidersContainer VideoProvidersContainer;

  Application();

  ~Application() override;

  inline FramesetConverter *converter() {
    return converter_;
  }

  inline dove_eye::Parameters &parameters() {
    return parameters_;
  }

 signals:
  void ChangedArity(const dove_eye::CameraIndex arity);

 public slots:
  VideoProvidersVector AvailableVideoProviders();
  void UseVideoProviders(const VideoProvidersVector &providers);
// TODO think about this
// - rather rename (func pointer resolution)
  //void UseVideoProviders(const std::string &filename);

 private:
  dove_eye::CameraIndex arity_;
  dove_eye::Parameters parameters_;
  VideoProvidersVectorOwning available_providers_;

  Controller *controller_;
  FramesetConverter* converter_;

  std::unique_ptr<dove_eye::Tracker> tracker_;
  std::unique_ptr<dove_eye::Localization> localization_;
  std::unique_ptr<dove_eye::TemplateTracker> inner_tracker_;

  QList<QThread *> threads_;
  QList<QObject *> objects_in_threads_;


  void MoveToNewThread(QObject* object);
  void MoveToThread(QObject* object, QThread* thread);

  void SetArity(const dove_eye::CameraIndex arity);

  void SetupController(VideoProvidersContainer &&providers);
  void SetupConverter();

  template<typename T>
  void SwapAndDestroy(T **object_ptr, T *new_object) {
    assert(object_ptr);
    /*
     * Queue old object for destruction (it's safe to call it again in
     * application's dtor), then we replace it with the new converter and reuse
     * original's thread.
     */
    T *old_object = *object_ptr;

    if (old_object != nullptr) {
      auto thread = old_object->thread();
      old_object->deleteLater();
      MoveToThread(new_object, thread);
    } else {
      MoveToNewThread(new_object);
    }

    *object_ptr = new_object;
  }
};


#endif // APPLICATION_H_
