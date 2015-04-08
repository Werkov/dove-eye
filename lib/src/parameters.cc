#include "dove_eye/parameters.h"

#include <sstream>

#define DEFINE_PARAM(KEY, NAME, DEFAULT, UNIT, MIN_VAL, MAX_VAL) \
    {Parameters::KEY, Parameters::KEY, NAME, DEFAULT, UNIT,      \
                 MIN_VAL, MAX_VAL }

#define DEFINE_PARAM_ARRAY(KEY, NAME, DEFAULT, UNIT, MIN_VAL, MAX_VAL) \
    {Parameters::KEY, Parameters::_LAST_KEY_NAME(KEY), NAME, DEFAULT,  \
                 UNIT, MIN_VAL, MAX_VAL }

using std::stringstream;

namespace dove_eye {

const Parameters::Parameter Parameters::parameters[] = {
  DEFINE_PARAM(
      TEMPLATE_RADIUS,        "template.radius",        15,       "px", 2, 100 ),
  DEFINE_PARAM(
      TEMPLATE_SEARCH_FACTOR, "template.search_factor", 2,         "",    1, 3 ),
  DEFINE_PARAM(
      TEMPLATE_THRESHOLD,     "template.threshold",     0.5,       "",    0, 1 ),
  DEFINE_PARAM(
      AGGREGATOR_WINDOW,      "aggregator.window",      0.1,      "s",   0, 5 ),
  DEFINE_PARAM_ARRAY(
      CAM_OFFSET,             "aggregator.offset",      0,        "s",   0, 5 ),
  DEFINE_PARAM(
      CALIBRATION_ROWS,       "calibration.rows",       6,         "",    1, 10 ),
  DEFINE_PARAM(
      CALIBRATION_COLS,       "calibration.cols",       9,         "",    1, 10 ),
  DEFINE_PARAM(
      CALIBRATION_SIZE,       "calibration.size",   0.026,        "m", 1e-2, 1e-1 ),
  DEFINE_PARAM(
      CALIBRATION_FRAMES,     "calibration.frames",    10, "frame(s)",   10, 100 ),
  DEFINE_PARAM(
      CALIBRATION_SKIP,       "calibration.skip",      15, "frame(s)",    0, 50  ),
  
  {Parameters::_MAX_KEY, Parameters::_MAX_KEY}
};

Parameters::Parameters() {
  size_t i = 0;
  while (true) {
    const Parameter &param = Parameters::parameters[i];

    if (param.key == Parameters::_MAX_KEY) {
      break;
    }

    for (size_t key = param.key; key <= param.last_key; ++key) {
      parameters_[key] = param;
      parameters_[key].key = static_cast<Key>(key);

      if (param.key != param.last_key) {
        stringstream ss;
        ss << parameters_[key].name;
        ss << "[" << (key - param.key) << "]";
        parameters_[key].name = ss.str();
      }
    }
    ++i;
  }
}

bool Parameters::Set(const Key key, const double value) {
  assert(key < _MAX_KEY);
  
  std::lock_guard<std::mutex> lock(parameters_mtx_);
  if (value < parameters_[key].min_value ||
      value > parameters_[key].max_value) {
    return false;
  }

  parameters_[key].value = value;
  return true;
}

double Parameters::Get(const Key key) const {
  assert(key < _MAX_KEY);
  
  /* Alas in C++11, no shared_lock available... */
  std::lock_guard<std::mutex> lock(parameters_mtx_);
  return parameters_[key].value;
}


double Parameters::Get(const Key key, const size_t offset) const {
  auto new_key = static_cast<size_t>(key) + offset;
  return Get(static_cast<Key>(new_key));
}

} // namespace dove_eye
