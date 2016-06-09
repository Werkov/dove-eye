#include "tld_tracker.h"
#include "TLDUtil.h"

namespace dove_eye {
	bool TldTracker::InitTrackerData(const cv::Mat &data, const Mark &mark) {
		assert(mark.type == Mark::kRectangle);
		tracker_data();
		data_.grey = cv::Mat(data.rows, data.cols, CV_8UC1);

		return true;
	}

	bool TldTracker::InitializeTracking(const Frame &frame, const Mark mark,
		Posit *result) {

		if (!InitTrackerData(frame.data, mark)) {
			return false;
		}
		
		cvtColor(frame.data, data_.grey, CV_BGR2GRAY);

		tld->detectorCascade->imgWidth = data_.grey.cols;
		tld->detectorCascade->imgHeight = data_.grey.rows;
		tld->detectorCascade->imgWidthStep = data_.grey.step;

		int *initialBB = new int[4];

		initialBB[0] = mark.top_left.x;
		initialBB[1] = mark.top_left.y;
		initialBB[2] = mark.size.x;
		initialBB[3] = mark.size.y;
		cv::Rect bb = tld::tldArrayToRect(initialBB);

		tld->selectObject(data_.grey, &bb);

		*result = MarkToPosit(mark);

		return true;
	}

	bool TldTracker::InitializeTracking(
		const Frame &frame,
		const Epiline epiline,
		const TrackerData &tracker_data,
		Posit *result) {
		
		return false;

		//auto thickness = parameters().Get(Parameters::TEMPLATE_RADIUS) *
		//	parameters().Get(Parameters::SEARCH_FACTOR);

		//const auto epiline_mask = EpilineToMask(frame.data.size(), thickness, epiline);

		//Mark match_mark;
		//// TODO Remove non-const cast! Do it properly when projection is working
		//if (!Search(frame.data, (TrackerData &)tracker_data, nullptr, &epiline_mask, thr,
		//	&match_mark)) {
		//	return false;
		//}

		//if (!InitTrackerData(frame.data, match_mark)) {
		//	return false;
		//}
		//initialized(true);

		//*result = MarkToPosit(match_mark);

		//return true;
	}

	bool TldTracker::Track(const Frame &frame, Posit *result) {
		assert(initialized());

		RunTracking(frame, result);

		return true;
	}

	bool TldTracker::ReinitializeTracking(const Frame &frame, Posit *result) {
		assert(initialized());

		RunTracking(frame, result);

		return true;
	}

	void TldTracker::RunTracking(const Frame &frame, Posit *result) {
		if (!data_.reuseFrameOnce)
		{
			cvtColor(frame.data, data_.grey, CV_BGR2GRAY);
		}
		else
		{
			data_.reuseFrameOnce = false;
		}

		if (!data_.skipProcessingOnce)
		{
			tld->processImage(frame.data);
		}
		else
		{
			data_.skipProcessingOnce = false;
		}

		Mark mark;
		mark.top_left.x = tld->currBB->x;
		mark.top_left.y = tld->currBB->y;
		mark.size.x = tld->currBB->width;
		mark.size.x = tld->currBB->height;

		*result = MarkToPosit(mark);
	}
}