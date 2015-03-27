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
    TEMPLATE_RADIUS,
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

  const static Parameter parameters[];

  Parameters();

  double Get(const Key key) const;

  bool Set(const Key key, const double value);

 private:
  mutable std::mutex parameters_mtx_;
  std::array<Parameter, _MAX_KEY> parameters_;
  std::array<double, _MAX_KEY> values_;
};

} // namespace dove_eye

#endif // DOVE_EYE_PARAMETERS_H_
