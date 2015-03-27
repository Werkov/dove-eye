#ifndef DOVE_EYE_PARAMETERS_H_
#define DOVE_EYE_PARAMETERS_H_

namespace dove_eye {

class Parameters {
 public:
  enum Key {
    TEMPLATE_RADIUS,
    TEMPLATE_SEARCH_FACTOR,
    TEMPLATE_THRESHOLD
  };

  template<typename T = double>
  T Get(const Key key) const {
    switch (key) {
      case TEMPLATE_RADIUS:
        return 30;
      case TEMPLATE_SEARCH_FACTOR:
        return 2;
      case TEMPLATE_THRESHOLD:
        return 0.5;
      default:
        assert(false);
    }
  }

};

} // namespace dove_eye

#endif // DOVE_EYE_PARAMETERS_H_
