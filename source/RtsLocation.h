#ifndef RTSLOCATION_H_
#define RTSLOCATION_H_

#include <opencv2/opencv.hpp>
#include <string>

class RtsLocation {
public:
    RtsLocation(cv::Point ul, cv::Point br, std::string n);

    void setCenterPoint(void);
    int pointExistsIn(cv::Point c);
    void displayInfo(cv::Mat *ipMap, cv::Point iCurrentLocation);

private:
    cv::Point upperLeft;
    cv::Point bottomRight;
    cv::Point center;
    std::string name;
};

#endif // RTSLOCATION_H_