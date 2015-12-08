#ifndef DOVE_EYE_SEARCHING_TRACKER_H_
#define DOVE_EYE_SEARCHING_TRACKER_H_

#include <memory>
#include <opencv2/opencv.hpp>

#include "dove_eye/inner_tracker.h"
#include "dove_eye/cv_kalman_filter.h"
#include "dove_eye/parameters.h"

namespace dove_eye {

class SearchingTracker : public InnerTracker {
 public:
  explicit SearchingTracker(const Parameters &parameters)
      : InnerTracker(parameters),
        initialized_(false) {
  }

  bool InitializeTracking(const Frame &frame, const Mark mark,
                          Posit *result) override;

  bool InitializeTracking(
      const Frame &frame,
      const Epiline epiline,
      const TrackerData &tracker_data,
      Posit *result) override;
 
  bool Track(const Frame &frame, Posit *result) override;

  // FIXME override other ReinitializeTracking overloads
  bool ReinitializeTracking(const Frame &frame, Posit *result) override;

 protected:
  typedef CvKalmanFilter KalmanFilterT;

  inline bool initialized() const  {
    return initialized_;
  }

  inline KalmanFilterT &kalman_filter() {
    return kalman_filter_;
  }

  inline cv::BackgroundSubtractor &bg_subtractor() {
    return bg_subtractor_;
  }


  /** Initialize tracker data
   * @return  false when initialization failed
   */
  virtual bool InitTrackerData(const cv::Mat &data, const Mark &mark) = 0;

  /** Generic search for an matching object
   *
   * @param[in]   data         image to search for object
   * @param[in]   tracker_data search query (implementation specific)
   * @param[in]   roi          (optional) region of interest that should be searched
   *                           (in the image)
   * @param[in]   mask         (optional) boolean mask restricting search (in the
   *                           image too)
   * @param[in]   threshold    value [0,1] to accept the match (the higher, the
   *                           better)
   * @param[out]  result       mark positioned to the best match
   *
   * @return      true if sufficient match was found, false otherwise
   */
  virtual bool Search(
      const cv::Mat &data,
      const TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result) const = 0;

  virtual Posit MarkToPosit(const Mark &mark) const = 0;

  virtual cv::Rect DataToRoi(const TrackerData &tracker_data, const Point2 exp,
                             const double search_factor) const = 0;

 private:
  bool initialized_;
  KalmanFilterT kalman_filter_;
  cv::BackgroundSubtractorMOG bg_subtractor_;

  inline void initialized(const bool value) {
    initialized_ = value;
  }

  void InitializeKalmanFilter();
};

} // namespace dove_eye

#endif // DOVE_EYE_SEARCHING_TRACKER_H_
