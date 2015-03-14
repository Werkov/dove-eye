#include "dove_eye/frame.h"

namespace dove_eye {

Frame Frame::Clone() const {
  Frame result(*this);
  result.data = data.clone();
  return result;
}

} // namespace dove_eve
