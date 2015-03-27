#ifndef DOVE_EYE_PARAMETERS_H_
#define DOVE_EYE_PARAMETERS_H_

#include <array>
#include <cassert>
#include <mutex>
#include <string>

namespace dove_eye {

class Parameters {
 public:
  enum Key {
    TEMPLATE_RADIUS = 0,
    TEMPLATE_SEARCH_FACTOR,
    TEMPLATE_THRESHOLD,
    _MAX_KEY
  };

  struct Parameter {
    Key key;
    std::string name;
    double default_value;
    std::string unit;
    double min_value;
    double max_value;
  };

  class iterator {
   public:
    explicit iterator(const size_t key)
        : key_(key) {
    }

    inline const Parameter &operator*() const {
      return Parameters::parameters[key_];
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
  };

  const static Parameter parameters[];

  Parameters();

  inline iterator begin() const {
    return iterator(0);
  }

  inline iterator end() const {
    return iterator(static_cast<size_t>(_MAX_KEY));
  }

  double Get(const Key key) const;

  bool Set(const Key key, const double value);

 private:
  mutable std::mutex parameters_mtx_;
  std::array<Parameter, _MAX_KEY> parameters_;
  std::array<double, _MAX_KEY> values_;
};

} // namespace dove_eye

#endif // DOVE_EYE_PARAMETERS_H_
