#ifndef DOVE_EYE_ASYNC_POLICY_H_
#define DOVE_EYE_ASYNC_POLICY_H_

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/frameset_aggregator.h"
#include "dove_eye/types.h"
#include "dove_eye/logging.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {

template<bool allow_drop>
class AsyncPolicy {
 public:
  typedef std::vector<VideoProvider *> ProvidersContainer;

  explicit AsyncPolicy(const ProvidersContainer &providers)
      : providers_(providers),
        threads_(providers_.size()),
        max_queue_size_(providers_.size() * kQueueSizeFactor_) {
  }

  ~AsyncPolicy() {
    stop_requested_ = true;
    queue_cv_.notify_all();

    for (auto &thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  void Start() {
    running_producers_ = providers_.size();
    stop_requested_ = false;

    for (CameraIndex cam = 0; cam < providers_.size(); ++cam) {
      threads_[cam] = std::thread(&AsyncPolicy::ReadProvider, this, cam);
    }
  }

  bool GetFrame(Frame *frame, CameraIndex *cam) {
    Lock lock(queue_mtx_);


    queue_cv_.wait(lock, [&] {
                    return queue_.size() > 0 || running_producers_ == 0;
                   });

    if (running_producers_ == 0) {
      return false;
    }

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
  size_t running_producers_;

  std::atomic<bool> stop_requested_;

  std::queue<CamFrame> queue_;
  std::mutex queue_mtx_;
  std::condition_variable queue_cv_;


  void ReadProvider(const CameraIndex cam) {
    for (auto frame : *providers_[cam]) {
      /* Note the lock is released on every iteration */
      Lock lock(queue_mtx_);

      if (queue_.size() == max_queue_size_) {
        if (allow_drop) {
          continue;
        } else {
          queue_cv_.wait(lock, [&] {
                          return (queue_.size() < max_queue_size_) ||
                              stop_requested_;
                        });
        }
      }

      if (stop_requested_) {
        break;
      }

      queue_.push(CamFrame(frame, cam));
      queue_cv_.notify_all();
    }

    {
      Lock lock(queue_mtx_);
      assert(running_producers_ > 0);
      running_producers_ -= 1;
      queue_cv_.notify_all();
    }
  }
};


} // namespace dove_eye

#endif // DOVE_EYE_ASYNC_POLICY_H_

