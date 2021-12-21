
#ifndef RTSOBJECTLIST_H_
#define RTSOBJECTLIST_H_

#include <list>
#include <opencv2/opencv.hpp>

#include "RtsObject.h"

class RtsObjectList {
public:
    RtsObjectList();

    void initRtsObjectList(int numberofobjects);
    void pushObject(RtsObject &irObject);
    void pushObjects(RtsObjectList &irObjectList);
    void displayObjects(cv::Mat *background);
    void displayStaticObjects(cv::Mat *background);
    void displayDynamicObjects(cv::Mat *background);
    int isObjectMotion(int which);
    void applyMotion(std::vector<cv::Point> &irFlowPoints);

private:
    std::list<RtsObject> objectList;
};

#endif // RTSOBJECTLIST_H_