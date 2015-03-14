#ifndef DOVE_EYE_FILE_VIDEO_PROVIDER_H_
#define DOVE_EYE_FILE_VIDEO_PROVIDER_H_

#include <memory>
#include <mutex>
#include <string>

#include <opencv2/opencv.hpp>

#include "dove_eye/video_provider.h"

namespace dove_eye {

class FileVideoProvider : public VideoProvider {
 public:
  explicit FileVideoProvider(const std::string &filename,
                             const bool blocking = true);

  FrameIterator begin() override;

  FrameIterator end() override;

 private:
  class Iterator : public FrameIteratorImpl {
   public:
    explicit Iterator(const FileVideoProvider &provider);

    Frame GetFrame() const override;

    void MoveNext() override;

    bool IsValid() override {
      return valid_;
    }

   private:
    struct CaptureDeleter {
      void operator()(cv::VideoCapture *to_delete) const;
    };
    static CaptureDeleter capture_deleter_;

    typedef std::unique_ptr<cv::VideoCapture, Iterator::CaptureDeleter>
        CvCapturePtr;
    typedef std::lock_guard<std::mutex> CaptureLock;

    Iterator(const Iterator &);
    /**
     * @see http://stackoverflow.com/a/12243521/1351874
     */
    static std::mutex capture_lifecycle_mtx_;

    const FileVideoProvider &provider_;

    /* Unique ownership, cannot copy the iterator anyway. */
    CvCapturePtr video_capture_;

    bool valid_;
    Frame frame_;
    int frame_no_;
    double frame_period_;
  };

  const std::string filename_;

  /** Should retrieval of frames be blocking (for period derived from FPS). */
  const bool blocking_;
};

} // namespace dove_eye

#endif // DOVE_EYE_FILE_VIDEO_PROVIDER_H_

