#ifndef RTSOBJECT_H_
#define RTSOBJECT_H_

#include "RtsCommon.h"
#include <opencv2/opencv.hpp>

class RtsObject {
public:
    // this function is called on
    // RtsObject foo = new RtsObject(parameters)
    RtsObject(cv::Mat &irImage, ImageTypeEnum iType, cv::Point init_center,
              ModuleTypeEnum iModuleType, int i);

    void findCenterPoint(void);
    void checkBounds(void);
    void findUpperLeftPoint(cv::Point center);
    void findBottomRightPoint(cv::Point center);
    void renderObject(cv::Mat *background);
    void setCenterPoint(void);

    int applyFlow(std::vector<cv::Point> &irFlowPoints);
    int moveObjectToPoint(cv::Point iUpperLeftPoint);
    int adjustVelocity(cv::Point velocity);

    int getGravity(void) { return gravity; }
    double getFriction(void) { return friction; }
    double getElasticity(void) { return elasticity; }
    bool hasMotion(void) { return motion; }
    bool IsMoving(void) { return moving; }

    cv::Point GetUpperLeft() { return upperLeft; }
    cv::Point GetBottomRight() { return bottomRight; };
    cv::Point GetVelocity() { return velocity; }
    cv::Mat &GetImage() { return rImage; }

private:
    int type;
    int id; // id is used to differ from certain sliders and images, use
            // intelligently.
    ModuleTypeEnum module;
    bool motion; // tell if it has a corresponding 'motion' point 0 or 1
    bool moving; // tell if the object will be controlled by physics
    int gravity;
    double friction;
    double elasticity;
    cv::Point upperLeft;
    cv::Point bottomRight;
    cv::Point center;
    cv::Point velocity;
    cv::Mat &rImage;
};

#endif // RTSOBJECT_H_