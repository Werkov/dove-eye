#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>
#include <vector>

#include <QEventLoop>
#include <QList>
#include <QObject>
#include <QThread>

#include "controller.h"
#include "dove_eye/aggregator.h"
#include "dove_eye/calibration_data.h"
#include "dove_eye/parameters.h"
#include "dove_eye/localization.h"
#include "dove_eye/types.h"
#include "dove_eye/video_provider.h"
#include "frameset_converter.h"
#include "io/calibration_data_storage.h"
#include "io/parameters_storage.h"

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
  typedef dove_eye::Aggregator::ProvidersContainer VideoProvidersContainer;

  enum ProvidersType {
    kNoProviders,
    kCameras,
    kVideoFiles
  };

  Application();

  ~Application() override;

  inline dove_eye::CameraIndex Arity() const {
    return arity_;
  }

  inline ProvidersType providers_type() const {
    return providers_type_;
  }

  inline dove_eye::Parameters &parameters() {
    return parameters_;
  }

  inline dove_eye::CalibrationData &calibration_data() {
    return *calibration_data_;
  }

  inline io::ParametersStorage *parameters_storage() {
    return &parameters_storage_;
  }

  inline io::CalibrationDataStorage *calibration_data_storage() {
    return &calibration_data_storage_;
  }

  inline Controller *controller() {
    return controller_;
  }

  inline FramesetConverter *converter() {
    return converter_;
  }

  // TODO replace this pointer harakiri with properly encapsulated class
  //      ProvidersRepo that'll support ownership and outer access.
  inline VideoProvidersVectorOwning *ProvidersContainer() {
    return &available_providers_;
  }

 signals:
  void SetupPipeline();
  // TODO implement loading from file
  void CalibrationDataReady(const dove_eye::CalibrationData);

 public slots:
  VideoProvidersVector ScanCameraProviders();
  void ReleaseAvailableProviders();

  void InitializeEmpty();

  void Initialize(const ProvidersType type,
                  const VideoProvidersVector &providers);

  void SetCalibrationData(const dove_eye::CalibrationData calibration_data);

 private:
  dove_eye::CameraIndex arity_;
  dove_eye::Parameters parameters_;
  std::unique_ptr<dove_eye::InnerTracker> inner_tracker;
  ProvidersType providers_type_;
  VideoProvidersVectorOwning available_providers_;
  std::unique_ptr<dove_eye::CalibrationData> calibration_data_;

  io::ParametersStorage parameters_storage_;
  io::CalibrationDataStorage calibration_data_storage_;

  Controller *controller_;
  FramesetConverter* converter_;

  QList<QThread *> threads_;
  QList<QObject *> objects_in_threads_;


  void MoveToNewThread(QObject* object);
  void MoveToThread(QObject* object, QThread* thread);

  void SetupController(const ProvidersType type,
                       VideoProvidersContainer &&providers);
  void TeardownController();
  void SetupConverter();
  std::unique_ptr<dove_eye::InnerTracker> SetupTracker(int i);
  void TeardownConverter();

  template<typename T>
  void SwapAndDestroy(T **object_ptr, T *new_object, bool blocking = false) {
    assert(object_ptr);
    /*
     * Queue old object for destruction (it's safe to call it again in
     * application's dtor), then we replace it with the new converter and reuse
     * original's thread.
     */
    T *old_object = *object_ptr;

    if (old_object != nullptr) {
      auto obj_thread = old_object->thread();

      objects_in_threads_.removeAll(old_object);
      if (blocking) {
        old_object->deleteLater();
        /* Ugly hack:
         * We cannot technically wait for actual destruction of the object, the
         * destroyed signal is emitted right *before* the member fields are
         * destroyed and member fields are those that could block (e.g.
         * Aggregator and its AsyncPolicy. Thus use sleep "ensure" the object
         * is properly destroyed.
         */
        thread()->msleep(200);
      } else {
        old_object->deleteLater();
      }

      if (new_object != nullptr) {
        MoveToThread(new_object, obj_thread);
      }
    } else if (new_object != nullptr) {
      MoveToNewThread(new_object);
    }

    *object_ptr = new_object;
  }
};


#endif // APPLICATION_H_
