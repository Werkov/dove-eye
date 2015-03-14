#ifndef DOVE_EYE_ASYNC_POLICY_H_
#define DOVE_EYE_ASYNC_POLICY_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/types.h"
#include "dove_eye/logging.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {

template<bool allow_drop>
class AsyncPolicy {
 public:
  typedef std::vector<std::unique_ptr<VideoProvider>> ProvidersContainer;

  explicit AsyncPolicy(ProvidersContainer &&providers)
      : providers_(std::move(providers)),
        threads_(providers_.size()),
        max_queue_size_(providers_.size() * kQueueSizeFactor_) {
  }

  ~AsyncPolicy() {
    for (auto &thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  void Start() {
    producers_ = providers_.size();

    for (CameraIndex cam = 0; cam < providers_.size(); ++cam) {
      threads_[cam] = std::thread(&AsyncPolicy::ReadProvider, this, cam);
    }
  }

  bool GetFrame(Frame *frame, CameraIndex *cam) {
    Lock lock(queue_mtx_);

    if (producers_ == 0) {
      DEBUG("Finished\n");
      return false;
    }

    queue_cv_.wait(lock, [&] {
                    return queue_.size() > 0;
                   });
    auto cam_frame = queue_.front();
    queue_.pop();
    queue_cv_.notify_all();
    lock.unlock();

    *frame = cam_frame.first;
    *cam = cam_frame.second;
    return true;
  }

 private:
  typedef std::vector<std::thread> ThreadContainer;
  typedef std::unique_lock<std::mutex> Lock;
  typedef std::pair<Frame, CameraIndex> CamFrame;

  static const size_t kQueueSizeFactor_ = 2;

  ProvidersContainer providers_;
  ThreadContainer threads_;

  const size_t max_queue_size_;
  /* It's synchronized by queue_mtx_ too. */
  size_t producers_;

  std::queue<CamFrame> queue_;
  std::mutex queue_mtx_;
  std::condition_variable queue_cv_;

  void ReadProvider(const CameraIndex cam) {
    Lock lock(queue_mtx_, std::defer_lock);

    for (auto frame : *providers_[cam]) {
      lock.lock();
      if (queue_.size() == max_queue_size_) {
        if (allow_drop) {
          DEBUG("AsyncPolicy dropped a frame\n");
          lock.unlock();
          continue;
        } else {
          queue_cv_.wait(lock, [&] {
                          return queue_.size() < max_queue_size_;
                        });
        }
      }

      DEBUG("Obtained frame from cam %i\n", cam);
      queue_.push(CamFrame(frame, cam));
      queue_cv_.notify_all();
      lock.unlock();
    }

    lock.lock();
    assert(producers_ > 0);
    producers_ -= 1;
    lock.unlock();
  }
};


} // namespace dove_eye

#endif // DOVE_EYE_ASYNC_POLICY_H_

