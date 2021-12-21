#include "RtsObjectList.h"

RtsObjectList::RtsObjectList() : objectList() {}

void RtsObjectList::pushObject(RtsObject &irObject) {
    objectList.push_back(irObject);
}

void RtsObjectList::pushObjects(RtsObjectList &irObjectList) {
    for (auto &object : irObjectList.objectList) {
        pushObject(object);
    }
}

void RtsObjectList::displayObjects(cv::Mat *background) {
    for (auto &object : objectList) {
        object.renderObject(background);
    }
}

void RtsObjectList::displayStaticObjects(cv::Mat *background) {
    for (auto &object : objectList) {
        if (object.hasMotion() == 0) {
            object.renderObject(background);
        }
    }
}

void RtsObjectList::displayDynamicObjects(cv::Mat *background) {
    for (auto &object : objectList) {
        if (object.hasMotion() == 1) {
            object.renderObject(background);
        }
    }
}

void RtsObjectList::applyMotion(std::vector<cv::Point> &irFlowPoints) {
    for (auto &object : objectList) {
        if (object.hasMotion() == 0) {
        }
    }
}
