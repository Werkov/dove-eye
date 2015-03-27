#include "dove_eye/parameters.h"

namespace dove_eye {

const Parameters::Parameter Parameters::parameters[] = {
  {Parameters::TEMPLATE_RADIUS,        "template.radius",        30,   "px", 2, 100 },
  {Parameters::TEMPLATE_SEARCH_FACTOR, "template.search_factor", 2,    "",   1, 3 },
  {Parameters::TEMPLATE_THRESHOLD,     "template.threshold",     0.5,  "",   0, 1 },
  {Parameters::_MAX_KEY}
};

Parameters::Parameters() {
  for (auto param : *this) {
    parameters_[param.key] = param;
    values_[param.key] = param.default_value;
  }
}

bool Parameters::Set(const Key key, const double value) {
  assert(key < _MAX_KEY);
  
  std::lock_guard<std::mutex> lock(parameters_mtx_);
  if (value < parameters_[key].min_value ||
      value > parameters_[key].max_value) {
    return false;
  }

  values_[key] = value;
  return true;
}

double Parameters::Get(const Key key) const {
  assert(key < _MAX_KEY);
  
  /* Alas in C++11, no shared_lock available... */
  std::lock_guard<std::mutex> lock(parameters_mtx_);
  return values_[key];
}

} // namespace dove_eye
