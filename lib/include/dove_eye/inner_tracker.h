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
      : parameters_(parameters) {
  }

  virtual ~InnerTracker() {
  }

  virtual const TrackerData &tracker_data() const = 0;

  virtual TrackerData &tracker_data() = 0;

  /** Global initialization from mark
   *
   * Reset internal state of tracker (for stateful trackers).
   */
  virtual bool InitializeTracking(const Frame &frame, const Mark mark,
                                  Posit *result) = 0;
  
  /** Epiline initialization on epiline with other tracker's data
   *
   * Reset internal state of tracker (for stateful trackers).
   */
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

 private:
  const Parameters &parameters_;
};

} // namespace dove_eye

#endif // DOVE_EYE_INNER_TRACKER_H_

