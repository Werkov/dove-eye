#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <vector>
#include <map>
#include <memory>

using namespace cv;
using namespace std;

const map<int,string> PROPERTIES = {
	{CV_CAP_PROP_POS_MSEC,		"CV_CAP_PROP_POS_MSEC"},
	{CV_CAP_PROP_POS_FRAMES, 	"CV_CAP_PROP_POS_FRAMES"},
	{CV_CAP_PROP_POS_AVI_RATIO, 	"CV_CAP_PROP_POS_AVI_RATIO"},
	{CV_CAP_PROP_FRAME_WIDTH, 	"CV_CAP_PROP_FRAME_WIDTH"},
	{CV_CAP_PROP_FRAME_HEIGHT, 	"CV_CAP_PROP_FRAME_HEIGHT"},
	{CV_CAP_PROP_FPS, 		"CV_CAP_PROP_FPS"},
	{CV_CAP_PROP_FOURCC, 		"CV_CAP_PROP_FOURCC"},
	{CV_CAP_PROP_FRAME_COUNT, 	"CV_CAP_PROP_FRAME_COUNT"},
	{CV_CAP_PROP_FORMAT,		"CV_CAP_PROP_FORMAT"},
	{CV_CAP_PROP_MODE,		"CV_CAP_PROP_MODE"},
	{CV_CAP_PROP_BRIGHTNESS,	"CV_CAP_PROP_BRIGHTNESS"},
	{CV_CAP_PROP_CONTRAST,		"CV_CAP_PROP_CONTRAST"},
	{CV_CAP_PROP_SATURATION,	"CV_CAP_PROP_SATURATION"},
	{CV_CAP_PROP_HUE,		"CV_CAP_PROP_HUE"},
	{CV_CAP_PROP_GAIN,		"CV_CAP_PROP_GAIN"},
	{CV_CAP_PROP_EXPOSURE,		"CV_CAP_PROP_EXPOSURE"},
	{CV_CAP_PROP_CONVERT_RGB,	"CV_CAP_PROP_CONVERT_RGB"},
	//{CV_CAP_PROP_WHITE_BALANCE, 	"CV_CAP_PROP_WHITE_BALANCE"},
	{CV_CAP_PROP_RECTIFICATION, 	"CV_CAP_PROP_RECTIFICATION"}
};

const int SIZES[][2] = {
		{160, 120},
		{320, 240},
		{640, 480},
		{320, 180},
		{640, 360},
		{1280, 720}
	};



class FpsMeter {
private:
	chrono::high_resolution_clock::time_point prev_;
	double prevVal_;
public:
	FpsMeter() : prev_(chrono::high_resolution_clock::now()), prevVal_(0) {
	}

	double sample() {
		auto now = chrono::high_resolution_clock::now();
		double fps = (1.0e6 / chrono::duration_cast<chrono::microseconds>(now - prev_).count());
		prev_ = now;
		// smoothing
		prevVal_ = prevVal_ * 0.2 + fps * 0.8;
		return prevVal_;
	}
};

class SizeTrackbar {
private:
	VideoCapture &cap_;

	static void on_change(int value, void *data) {
		auto that = static_cast<SizeTrackbar *>(data);
		int w = SIZES[value][0];
		int h = SIZES[value][1];
		that->cap_.set(CV_CAP_PROP_FRAME_WIDTH, w);
		that->cap_.set(CV_CAP_PROP_FRAME_HEIGHT, h);

		cout << "Set resolution: " << that->cap_.get(CV_CAP_PROP_FRAME_WIDTH) << " x "
			<< that->cap_.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
	}

public:
	SizeTrackbar(VideoCapture &cap, const string &window_name) :
	  cap_(cap)
	{
		auto track_name = "SIZE";
		createTrackbar(track_name, window_name, NULL, sizeof(SIZES)/sizeof(SIZES[0]), &SizeTrackbar::on_change, this);
	}
};

class Trackbar {
private:
	static const int STEPS = 100;
	VideoCapture &cap_;
	int prop_;

	static void on_change(int value, void *data) {
		auto that = static_cast<Trackbar *>(data);
		that->cap_.set(that->prop_, value / static_cast<double>(STEPS));
	}

public:
	Trackbar(VideoCapture &cap, const string &window_name, const int prop, const double initial_value) :
	  cap_(cap), prop_(prop)
	{
		const auto track_name = PROPERTIES.at(prop);
		createTrackbar(track_name, window_name, NULL, STEPS, &Trackbar::on_change, this);
		setTrackbarPos(track_name, window_name, static_cast<int>(initial_value * STEPS));
	}
};


int main(int, char **) {
	const string WIN_MAIN("main");
	const string WIN_CTRL("ctrl");

	VideoCapture cap(0); // open the default camera

	if(!cap.isOpened()) { // check if we succeeded
		cerr << "Cannot open device." << endl;
		return 1;
	}


	/* Prepare GUI */
	typedef unique_ptr<Trackbar> Trackbar_ptr;
	namedWindow(WIN_MAIN, WINDOW_AUTOSIZE | WINDOW_OPENGL);
	namedWindow(WIN_CTRL, WINDOW_AUTOSIZE);
	SizeTrackbar sizeTrackbar(cap, WIN_CTRL);
	vector<Trackbar_ptr> trackbars;

	for(auto pair: PROPERTIES) {
		auto value = cap.get(pair.first);
		if(pair.first == CV_CAP_PROP_FRAME_WIDTH || pair.first == CV_CAP_PROP_FRAME_HEIGHT || value <= 0) {
			continue;
		}
		trackbars.push_back(Trackbar_ptr(new Trackbar(cap, WIN_CTRL, pair.first, value)));
	}


	/* Run loop */
	FpsMeter fpsm;


	for(;;) {
		Mat frame;
		cap >> frame; // get a new frame from camera
		cerr << "\r" << fpsm.sample();

		imshow(WIN_MAIN, frame);

		waitKey(1);
	}

	// the camera will be deinitialized automatically in VideoCapture destructor

	return 0;
}
