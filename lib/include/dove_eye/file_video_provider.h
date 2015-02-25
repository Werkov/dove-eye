#ifndef DOVE_EYE_FILE_VIDEO_PROVIDER_H_
#define	DOVE_EYE_FILE_VIDEO_PROVIDER_H_

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

#include <dove_eye/video_provider.h>

namespace dove_eye {

class FileVideoProvider : public VideoProvider {
 public:
  FileVideoProvider(const std::string &filename);

  // Creates a new iterator, all copies share the state.
  virtual FrameIterator begin() override;

  virtual FrameIterator end() override;

 private:
  class Iterator : public FrameIteratorImpl {
   public:
    Iterator(const FileVideoProvider &provider);
    
    virtual Frame GetFrame() const override;

    virtual void MoveNext() override;
    
    virtual bool IsValid() override {
      return valid_;
    }

   private:
    Iterator(const Iterator &);
    /* Unique ownership, cannot copy the iterator anyway. */
    std::unique_ptr<cv::VideoCapture> video_capture_;
    bool valid_;
    Frame frame_;
    int frame_no_;
    double frame_period_;
  };

  std::string filename_;
};

} // namespace dove_eye

#endif	/* DOVE_EYE_FILE_VIDEO_PROVIDER_H_ */

