#include "RtsLocation.h"

RtsLocation::RtsLocation(cv::Point ul, cv::Point br, std::string n) {
    this->upperLeft = ul;
    this->bottomRight = br;
    this->name.assign(n);
    this->setCenterPoint();
}

void RtsLocation::setCenterPoint(void) {
    this->center.x = (this->bottomRight.x - this->upperLeft.x) / 2;
    this->center.y = (this->bottomRight.y - this->upperLeft.y) / 2;
}

int RtsLocation::pointExistsIn(cv::Point c) {
    // if the shoe fits
    if ((c.x > this->upperLeft.x) && (c.y > this->upperLeft.y) &&
        (c.x < this->bottomRight.x) && (c.y < this->bottomRight.y))
        return 1;
    // else it doesn't
    return 0;
}
void RtsLocation::displayInfo(cv::Mat *ipMap, cv::Point iCurrentLocation) {
    char x[1024];
    char y[1024];
    char p[1024];
    float distance_x = abs(this->center.x - iCurrentLocation.x);
    float distance_y = abs(this->center.y - iCurrentLocation.y);
    float distance_p = sqrt(distance_x * distance_x + distance_y * distance_y);
    _itoa(distance_x, x, 10);
    _itoa(distance_y, y, 10);
    _itoa(distance_p, p, 10);

    ipMap->setTo(cv::Scalar(0, 0, 0));
    cv::putText(*ipMap, this->name.data(), cv::Point(10, 10),
                cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255, 0));
    cv::putText(*ipMap, "Distance X:", cv::Point(10, 100),
                cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255, 0));
    cv::putText(*ipMap, x, cv::Point(200, 100), cv::FONT_HERSHEY_DUPLEX, 1.0,
                cv::Scalar(255, 255, 255, 0));
    cv::putText(*ipMap, "Distance Y:", cv::Point(10, 150),
                cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255, 0));
    cv::putText(*ipMap, y, cv::Point(200, 150), cv::FONT_HERSHEY_DUPLEX, 1.0,
                cv::Scalar(255, 255, 255, 0));
    cv::putText(*ipMap, "Distance P:", cv::Point(10, 200),
                cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(255, 255, 255, 0));
    cv::putText(*ipMap, p, cv::Point(200, 200), cv::FONT_HERSHEY_DUPLEX, 1.0,
                cv::Scalar(255, 255, 255, 0));
}