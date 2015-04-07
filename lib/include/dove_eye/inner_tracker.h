#ifndef DOVE_EYE_INNER_TRACKER_H_
#define DOVE_EYE_INNER_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/frame.h"
#include "dove_eye/parameters.h"
#include "dove_eye/positset.h"
#include "dove_eye/types.h"

namespace dove_eye {

/** Implementation specific data of tracker (e.g. shape, template,...)
 *
 * @note TrackerData can be destroyed only directly on particular
 *       implementation.
 */
struct TrackerData {
};

/** Abstract ancestor of single camera tracker */
class InnerTracker {
 public:
  /** Mark is an object in input frame that can initialize tracking */
  struct Mark {
    enum Type {
      kInvalid,
      kCircle,
      kRectangle
    };

    Type type;

    /* Circle */
    Point2 center;
    double radius;

    /* Rectangle */
    Point2 top_left;
    Point2 size;

    Mark(const Type type = kInvalid)
        : type(type) {
    }
  };


  /** Vector of a, b, c for image line ax + by + c = 0 */
  typedef Vector3 Epiline;

  explicit InnerTracker(const Parameters &parameters)
      : parameters_(parameters),
        mark_set_(false) {
  }

  virtual ~InnerTracker() {
  }

  inline void SetMark(const Mark mark) {
    mark_set_ = true;
    mark_ = mark;
  }

  virtual const TrackerData &tracker_data() const = 0;

  /** Global initialization from mark */
  virtual bool InitializeTracking(const Frame &frame, Posit *result) = 0;
  
  /** Epiline initialization from other tracker's data */
  virtual bool InitializeTracking(
      const Frame &frame,
      const Epiline epiline,
      const TrackerData &tracker_data,
      Posit *result) = 0;

  /** Track the given frame */
  virtual bool Track(const Frame &frame, Posit *result) = 0;

  /** Global reinitialization */
  virtual bool ReinitializeTracking(const Frame &frame, Posit *result) = 0;

  /** Reinitialization from epiline */
  virtual inline bool ReinitializeTracking(const Frame &frame,
                                    const Epiline epiline,
                                    Posit *result) {
    return ReinitializeTracking(frame, result);
  }

  /** Reinitalization from projection point guess */
  virtual inline bool ReinitializeTracking(const Frame &frame,
                                    const Point2 guess,
                                    Posit *result) {
    return ReinitializeTracking(frame, result);
  }

  /** Clone unitialized tracker
   * @see dove_eye::Tracker
   */
  virtual InnerTracker *Clone() const = 0;

 protected:
  cv::Mat EpilineToMask(const cv::Size size, const int thickness,
                        const Epiline epiline) const;

  inline const Parameters &parameters() const {
    return parameters_;
  }

  inline const Mark &mark() const {
    assert(mark_set());
    return mark_;
  }

  inline bool mark_set() const {
    return mark_set_;
  }

 private:
  const Parameters &parameters_;

  bool mark_set_;
  Mark mark_;

};

} // namespace dove_eye

#endif // DOVE_EYE_INNER_TRACKER_H_

