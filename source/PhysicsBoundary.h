#ifndef PHYSICSBOUNDARY_H_
#define PHYSICSBOUNDARY_H_

#include "RtsObject.h"
#include "RtsUi.h"
#include <algorithm>

using namespace std;

class BoundRect {
public:
    BoundRect(cv::Point UL, cv::Point BR, char whichSides);
    BoundRect(cv::Point UL, cv::Point BR);
    BoundRect();

    int isInside(cv::Point pt);
    void drawBoundRect(cv::InputOutputArray iImage, cv::Scalar rectColor);
    void whichSideOut(cv::Point wSOpt, int *sideIndexArray);
    void whichSideOut(cv::Point UL, cv::Point BR, int *sideIndexArray);

private:
    int left, right, up, down;
    char sides;
};

class PhysicsBoundary {
public:
    PhysicsBoundary(BoundRect firstRect, int numRectIn);
    void addRect(BoundRect extraRect);
    void drawBoundary(cv::InputOutputArray iImage, cv::Scalar boundaryColor);
    int isPointInside(cv::Point iPIpt);
    int isSquareInside(cv::Point UL, cv::Point BR);
    void whichRectInside(cv::Point wRIpt, int *rectIndexArray);
    void
    whichRectInside(cv::Point UL, cv::Point BR,
                    int *rectIndexArray); // Overloaded version for rectangles
    int moveObject(list<RtsObject>::iterator o);

private:
    int totalNumRect;
    int rectCount;
    BoundRect *rectangles;
};

#endif /* PHYSICSBOUNDARY_H_ */
