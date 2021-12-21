#include "CvFlowDetector.h"

CvFlowDetector::CvFlowDetector()
    : trackingFeatures(), pointsPrevious(), pointsNext(), prevImage() {}

void CvFlowDetector::configureTracking(cv::Mat iImg) {
    cv::Mat grayFrame;
    cv::cvtColor(iImg, grayFrame, CV_BGR2GRAY);

    cv::goodFeaturesToTrack(
        grayFrame,                          // the image
        trackingFeatures,                   // the output detected features
        GOOD_FEATURES_MAX_COUNT,            // the maximum number of features
        GOOD_FEATURES_QUALITY_LEVEL,        // quality level
        GOOD_FEATURES_MIN_DISTANCE_PIXELS); // min distance between two features
}

int CvFlowDetector::trackNextFrame(cv::Mat iFrame,
                                   std::vector<cv::Point> &orPointsNext) {
    /*
        calcOpticalFlowPyrLK(
prev_grey, grey, prev_pyramid, pyramid, points[0], points[1],
total_objects, cvSize(win_size, win_size), 4, status, 0,
cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03), flags);
flags |= CV_LKFLOW_PYR_A_READY;*/

    cv::Mat grayFrame;
    cv::cvtColor(iImg, grayFrame, CV_BGR2GRAY);

    calcOpticalFlowPyrLK(prevFrame, grayFrame, pointsPrevious, pointsNext, );
    prevFrame = grayFrame;
}