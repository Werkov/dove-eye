#ifndef DOVE_EYE_FILE_VIDEO_PROVIDER_H_
#define DOVE_EYE_FILE_VIDEO_PROVIDER_H_

#include <string>

#include "dove_eye/video_provider.h"

namespace dove_eye {

class FileVideoProvider : public VideoProvider {
 public:
  explicit FileVideoProvider(const std::string &filename);

  inline std::string Id() const {
    return filename_;
  }

  FrameIterator begin() override;

  FrameIterator end() override;

 private:
  const std::string filename_;

};

} // namespace dove_eye

#endif // DOVE_EYE_FILE_VIDEO_PROVIDER_H_

