#include <iostream>
#include <unistd.h>
#include <vector>

#include <dove_eye/video_provider.h>

#include "dove_eye/time_calibration.h"

using std::string;
using std::vector;
using DoveEye::TimeCalibration;
using DoveEye::FileVideoProvider;

int CalibrateTime(const vector<string> &filenames) {
    TimeCalibration calibration;
    vector<TimeCalibration::ValueType> result;
    
    for(auto filename: filenames) {
        FileVideoProvider provider(filename);
        calibration.Reset();
        
        while(!calibration.MeasureFrame(provider.GetFrame()));
        result.push_back(calibration.result());        
    }
    
    // TODO store result           
}

int main(int argc, char* argv[]) {
    string usage = "Usage: %0 [-t|-c]";
    
    return CalibrateTime({"foo.mp4", "test.mkv"});
    
}

