#ifndef DOVE_EYE_PARAMS_H_
#define DOVE_EYE_PARAMS_H_

#include <array>
#include <cassert>
#include <mutex>
#include <string>

#include "dove_eye/frameset.h"

#define _LAST_KEY_NAME(KEY) _##KEY##_LAST

#define DECLARE_PARAM(KEY)             \
    KEY

#define DECLARE_PARAM_ARRAY(KEY, SIZE) \
    KEY,                               \
    _LAST_KEY_NAME(KEY) = KEY + (SIZE) - 1

namespace dove_eye {

class Parameters {
 public:
  enum Key {
    DECLARE_PARAM(TEMPLATE_RADIUS) = 0, //TODO rename to MARK_RADIOUS or something
    DECLARE_PARAM(TEMPLATE_SEARCH_FACTOR),
    DECLARE_PARAM(TEMPLATE_THRESHOLD),
    DECLARE_PARAM(AGGREGATOR_WINDOW),
    DECLARE_PARAM_ARRAY(CAM_OFFSET, Frameset::kMaxArity),
    DECLARE_PARAM(CALIBRATION_ROWS),
    DECLARE_PARAM(CALIBRATION_COLS),
    DECLARE_PARAM(CALIBRATION_SIZE),
    DECLARE_PARAM(CALIBRATION_FRAMES),
    DECLARE_PARAM(CALIBRATION_SKIP),
    _MAX_KEY
  };

  struct Parameter {
    Key key;
    Key last_key;
    std::string name;
    double value;
    std::string unit;
    double min_value;
    double max_value;
  };

  class iterator {
   public:
    explicit iterator(const size_t key, const Parameters &parameters)
        : key_(key),
          parameters_(parameters) {
    }

    inline const Parameter &operator*() const {
      return parameters_.parameters_[key_];
    }

    inline iterator &operator++() {
      ++key_;
      return *this;
    }

    inline bool operator!=(const iterator &rhs) {
      return key_ != rhs.key_;
    }

   private:
    size_t key_;
    const Parameters &parameters_;
  };

  /** Parameter definitions */
  const static Parameter parameters[];

  Parameters();

  inline iterator begin() const {
    return iterator(0, *this);
  }

  inline iterator end() const {
    return iterator(static_cast<size_t>(_MAX_KEY), *this);
  }

  double Get(const Key key) const;

  double Get(const Key key, const size_t offset) const;

  bool Set(const Key key, const double value);

 private:
  mutable std::mutex parameters_mtx_;
  /** Actual parameters */
  std::array<Parameter, _MAX_KEY> parameters_;
};

} // namespace dove_eye

#endif // DOVE_EYE_PARAMS_H_
