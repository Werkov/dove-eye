#pragma once
#include "E:\diplomka\dove-eye\lib\include\dove_eye\inner_tracker.h"
#include "dove_eye/inner_tracker.h"
#include "dove_eye/cv_kalman_filter.h"
#include "tld.h"

namespace dove_eye {

	class TldTracker :
		public InnerTracker
	{
	public:
		struct TldData : public TrackerData {
			bool reuseFrameOnce;
			bool skipProcessingOnce;
			cv::Mat grey;

			TldData() {
				reuseFrameOnce = true;
				skipProcessingOnce = true;
			}
		};

		explicit TldTracker(const Parameters &parameters)
			: InnerTracker(parameters),
			initialized_(false) {
		}

		inline const TrackerData &tracker_data() const override {
			return data_;
		}

		inline TrackerData &tracker_data() override {
			return data_;
		}

		bool InitializeTracking(const Frame &frame, const Mark mark,
			Posit *result) override;

		bool InitializeTracking(
			const Frame &frame,
			const Epiline epiline,
			const TrackerData &tracker_data,
			Posit *result) override;

		/** Track the given frame */
		bool Track(const Frame &frame, Posit *result) override;

		/** Global reinitialization */
		bool ReinitializeTracking(const Frame &frame, Posit *result) override;

		/** Reinitialization from epiline */
		inline bool ReinitializeTracking(const Frame &frame,
			const Epiline epiline,
			Posit *result) {
			return ReinitializeTracking(frame, result);
		}

		/** Reinitalization from projection point guess */
		inline bool ReinitializeTracking(const Frame &frame,
			const Point2 guess,
			Posit *result) {
			return ReinitializeTracking(frame, result);
		}

		InnerTracker *Clone() const override;

		inline InnerTracker::Mark::Type PreferredMarkType() const {
			return InnerTracker::Mark::kRectangle;
		}

	protected:
		~TldTracker();

	private:
		bool initialized_;
		tld::TLD *tld;

		TldData data_;

		bool InitTrackerData(const cv::Mat &data, const Mark &mark);

		inline Posit MarkToPosit(const Mark &mark) const {
			assert(mark.type == Mark::kRectangle);
			return mark.top_left + 0.5 * mark.size;
		}

		inline bool initialized() const  {
			return initialized_;
		}

		inline void initialized(const bool value) {
			initialized_ = value;
		}

		void RunTracking(const Frame &frame, Posit *result);
	};

}