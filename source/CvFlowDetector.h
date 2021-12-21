#ifndef CV_FLOW_DETECTOR_H_
#define CV_FLOW_DETECTOR_H_

#include <opencv2/opencv.hpp>

#define GOOD_FEATURES_MAX_COUNT 10000
#define GOOD_FEATURES_QUALITY_LEVEL .01

#define GOOD_FEATURES_MIN_DISTANCE_PIXELS 10

class CvFlowDetector {
public:
    CvFlowDetector();
    void configureTracking(cv::Mat iImg);
    int trackNextFrame(cv::Mat iFrame, std::vector<cv::Point> &orPointsNext);

private:
    std::vector<cv::Point> trackingFeatures;
    std::vector<cv::Point> pointsPrevious;
    std::vector<cv::Point> pointsNext;

    cv::Mat prevFrame;
};

#endif /* CV_FLOW_DETECTOR_H_ */