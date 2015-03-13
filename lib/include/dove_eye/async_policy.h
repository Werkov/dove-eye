#ifndef DOVE_EYE_ASYNC_POLICY_H_
#define DOVE_EYE_ASYNC_POLICY_H_

#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/types.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {

class AsyncPolicy {
 public:
  typedef std::vector<std::unique_ptr<VideoProvider>> ProvidersContainer;

  explicit AsyncPolicy(ProvidersContainer &&providers)
      : providers_(std::move(providers)),
        threads_(providers_.size()) {
  }
  // TODO join threads in dtor

  void Start() {
    for (CameraIndex cam = 0; cam < providers_.size(); ++cam) {
      // TODO interrupted
      std::thread retrive_thread(std::bind(&AsyncPolicy::ReadProvider, this),
                                 providers_[cam].get());
      threads_[cam] = std::move(retrive_thread);
    }
  }

  bool GetFrame(Frame *frame, CameraIndex *cam) {
    // TODO read blocking queue
    return true;
  }

 private:
  typedef std::vector<std::thread> ThreadContainer;

  ProvidersContainer providers_;
  ThreadContainer threads_;


  void ReadProvider(VideoProvider *provider) {
    // TODO read provider until it's finished or end is signalled
  }
};


} // namespace dove_eye

#endif // DOVE_EYE_ASYNC_POLICY_H_

