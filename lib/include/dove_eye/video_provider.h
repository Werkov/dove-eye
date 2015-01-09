#ifndef DOVE_EYE_VIDEO_PROVIDER_H_
#define	DOVE_EYE_VIDEO_PROVIDER_H_

#include <string>

#include <dove_eye/frame.h>

namespace DoveEye {

class VideoProvider {
 public:
  virtual Frame GetFrame() = 0;

  std::string Id() {
    return id_;
  }
 protected:
  std::string id_;
};

class FileVideoProvider : public VideoProvider {
 public:
  FileVideoProvider(std::string filename);
  
  virtual Frame GetFrame();
};

} // namespace DoveEye

#endif	/* DOVE_EYE_VIDEO_PROVIDER_H_ */

