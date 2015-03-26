#ifndef DOVE_EYE_PARAMETERS_H_
#define DOVE_EYE_PARAMETERS_H_

namespace dove_eye {

class Parameters {
 public:
  enum Key {
    TEMPLATE_RADIUS,
    TEMPLATE_SEARCH_FACTOR
  };

  template<typename T = double>
  T Get(const Key key) const {
    // TODO
    return 0;
  }

};

} // namespace dove_eye

#endif // DOVE_EYE_PARAMETERS_H_
