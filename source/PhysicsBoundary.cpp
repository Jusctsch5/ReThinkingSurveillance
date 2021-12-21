// Physics.c
#include "PhysicsBoundary.h"

BoundRect::BoundRect(cv::Point UL, cv::Point BR, char whichSides)
    : left(UL.x), right(BR.x), up(UL.y), down(BR.y), sides(whichSides) {}

BoundRect::BoundRect(cv::Point UL, cv::Point BR)
    : left(UL.x), right(BR.x), up(UL.y), down(BR.y), sides(15) {}

BoundRect::BoundRect() : left(0), right(0), up(0), down(0), sides(0) {}

int BoundRect::isInside(cv::Point pt) {
    if ((left <= pt.x && pt.x <= right) && (up <= pt.y && pt.y <= down))
        return 1;
    return 0;
}

void BoundRect::drawBoundRect(cv::InputOutputArray iimage,
                              cv::Scalar rectColor) {
    if (sides == 15)
        cv::rectangle(iimage, cv::Point(left, up), cv::Point(right, down),
                      rectColor, 2);
    else {
        if (sides & 8)
            cv::line(iimage, cv::Point(left, up), cv::Point(right, up),
                     rectColor, 2);
        if (sides & 4)
            cv::line(iimage, cv::Point(right, up), cv::Point(right, down),
                     rectColor, 2);
        if (sides & 2)
            cv::line(iimage, cv::Point(right, down), cv::Point(left, down),
                     rectColor, 2);
        if (sides & 1)
            cv::line(iimage, cv::Point(left, down), cv::Point(left, up),
                     rectColor, 2);
    }
}

void BoundRect::whichSideOut(cv::Point wSOpt, int *sideIndexArray) {
    sideIndexArray[0] = (wSOpt.y < up);
    sideIndexArray[1] = (wSOpt.x > right);
    sideIndexArray[2] = (wSOpt.y > down);
    sideIndexArray[3] = (wSOpt.x < left);
}

void BoundRect::whichSideOut(cv::Point UL, cv::Point BR, int *sideIndexArray) {
    sideIndexArray[0] = (UL.y < up);
    sideIndexArray[1] = (BR.x > right);
    sideIndexArray[2] = (BR.y > down);
    sideIndexArray[3] = (UL.x < left);
}

PhysicsBoundary::PhysicsBoundary(BoundRect firstRect, int numRectIn) {
    totalNumRect = numRectIn;
    rectangles = new BoundRect[totalNumRect];
    rectangles[0] = firstRect;
    rectCount = 1;
}

void PhysicsBoundary::addRect(BoundRect extraRect) {
    // Note: Do not assign overlapping rectangles! Can cause anomalous behavior
    // in getNewPos.
    rectCount++;
    if (rectCount > totalNumRect)
        fprintf(stderr,
                "Trying to assign too many rectangles to a boundary. You tried "
                "to assign %d\n",
                rectCount);
    return;
    rectangles[rectCount - 1] = extraRect;
}

void PhysicsBoundary::drawBoundary(cv::InputOutputArray iimage,
                                   cv::Scalar boundaryColor) {
    for (int i = 0; i < totalNumRect; i++) {
        rectangles[i].drawBoundRect(iimage, boundaryColor);
    }
}

int PhysicsBoundary::isPointInside(cv::Point iPIpt) {
    int isInside = 0;
    for (int i = 0; i < totalNumRect; i++) {
        isInside = isInside || rectangles[i].isInside(iPIpt);
    }
    return isInside;
}

int PhysicsBoundary::isSquareInside(cv::Point UL, cv::Point BR) {
    cv::Point UR, BL;
    int ULInside = 0, URInside = 0, BRInside = 0, BLInside = 0;
    UR = cv::Point(BR.x, UL.y);
    BL = cv::Point(UL.x, BR.y);
    for (int i = 0; i < totalNumRect; i++) {
        ULInside = ULInside || rectangles[i].isInside(UL);
        URInside = URInside || rectangles[i].isInside(UR);
        BRInside = BRInside || rectangles[i].isInside(BR);
        BLInside = BLInside || rectangles[i].isInside(BL);
    }
    return (ULInside && URInside && BRInside && BLInside);
}

void PhysicsBoundary::whichRectInside(cv::Point wRIpt, int *rectIndexArray) {
    for (int i = 0; i < totalNumRect; i++) {
        rectIndexArray[i] = rectangles[i].isInside(wRIpt);
    }
}

void PhysicsBoundary::whichRectInside(cv::Point UL, cv::Point BR,
                                      int *rectIndexArray) {
    for (int i = 0; i < totalNumRect; i++) {
        rectIndexArray[i] = (rectangles[i].isInside(UL) &&
                             rectangles[i].isInside(cv::Point(BR.x, UL.y)) &&
                             rectangles[i].isInside(BR) &&
                             rectangles[i].isInside(cv::Point(UL.x, BR.y)));
    }
}

int PhysicsBoundary::moveObject(list<RtsObject>::iterator o) {
    int whichSides[4];
    int *whichRects;
    cv::Point ULlast, ULnext, ULstartOrBounce;
    int stepTotal, stepCount, stepCountSinceBounce;
    double stepX, stepY;
    int sHeight, sWidth;
    cv::Point pointOutOfBounds;
    cv::Point velFinish;
    whichRects = new int[totalNumRect];

    int gravity = o->getGravity();
    double friction = o->getFriction();
    double elasticity = o->getElasticity();
    sWidth = o->GetImage()->size().width;
    sHeight = o->GetImage()->size().height;

    velFinish.x = o->GetVelocity().x;
    velFinish.y =
        o->GetVelocity().y; // Note that we add the lasting effect of gravity to
                            // the velocity at the end of the function.

    cv::Point startUL = o->GetUpperLeft();
    cv::Point startBR = o->GetBottomRight();

    stepTotal =
        max(abs(velFinish.x),
            abs(velFinish.y + gravity / 2)); // Note: A linear approximation of
                                             // a real trajectory under gravity.
    stepX = velFinish.x / double(stepTotal);
    stepY = (velFinish.y + gravity / 2) / double(stepTotal);

    stepCount = 0; // The total
    stepCountSinceBounce = 0;

    ULlast =
        startUL; // Used to tell which side we just hit (went out of bounds on)
    ULstartOrBounce = startUL; // Used to calculate where to be moving from.

    // fprintf(stderr, "At start of moveSquare, stepX = %g and stepY = %g \n",
    // stepX, stepY);

    if (!isSquareInside(startUL, startBR)) {
        fprintf(stderr, "Yo! Don't start with a point outside your boundary!");
        return 0;
    }

    while (stepCount < (stepTotal - 1)) {
        /*Notes:
           Move square one step at a time. If it goes out of
           bounds, go back to last position (and stepCount). Find which side it
           left from. To do this, first find which points of square are no
           longer in bounds. Then find which rectangle they just left. Then find
           which side the next position is out of bounds with.Adjust velocity
           appropriately. Continue to calculate until all steps have been
           completed.
        */
        ULnext.x = ULstartOrBounce.x + stepX * (stepCountSinceBounce + 1);
        ULnext.y = ULstartOrBounce.y + stepY * (stepCountSinceBounce + 1);
        // fprintf(stderr, "ULlast.x = %d, ULlast.y = %d. ULnext.x = %d,
        // ULnext.y = %d. stepCount = %d. \n", ULlast.x, ULlast.y, ULnext.x,
        // ULnext.y, stepCount); fprintf(stderr, "stepX = %g stepY = %g \n",
        // stepX, stepY);

        if (!isSquareInside(
                ULnext,
                cv::Point(
                    ULnext.x + sWidth,
                    ULnext.y +
                        sHeight))) // Means we just hit (bounced off) a side
        {
            if (!isPointInside(ULnext)) {
                whichRectInside(ULlast, whichRects);
                pointOutOfBounds = ULnext;
            } else if (!isPointInside(cv::Point(ULnext.x + sWidth, ULnext.y))) {
                whichRectInside(cv::Point(ULlast.x + sWidth, ULlast.y),
                                whichRects);
                pointOutOfBounds = cv::Point(ULnext.x + sWidth, ULnext.y);
            } else if (!isPointInside(
                           cv::Point(ULnext.x + sWidth, ULnext.y + sHeight))) {
                whichRectInside(
                    cv::Point(ULlast.x + sWidth, ULlast.y + sHeight),
                    whichRects);
                pointOutOfBounds =
                    cv::Point(ULnext.x + sWidth, ULnext.y + sHeight);
            } else if (!isPointInside(
                           cv::Point(ULnext.x, ULnext.y + sHeight))) {
                whichRectInside(cv::Point(ULlast.x, ULlast.y + sHeight),
                                whichRects);
                pointOutOfBounds = cv::Point(ULnext.x, ULnext.y + sHeight);
            } else
                fprintf(stderr,
                        "No points are outside bounds, but total square is!");

            for (int i = 0; i < totalNumRect; i++) {
                if (whichRects[i]) {
                    rectangles[i].whichSideOut(pointOutOfBounds, whichSides);
                    if (whichSides[0] || whichSides[2]) {
                        velFinish.x = velFinish.x * friction;
                        velFinish.y = velFinish.y * elasticity * -1;
                    } else if (whichSides[1] || whichSides[3]) {
                        velFinish.x = velFinish.x * elasticity * -1;
                        velFinish.y = velFinish.y * friction;
                    } else
                        fprintf(stderr,
                                "Whoa! Apparently square totally fits inside "
                                "rectangle %d; contradiction in moveSquare! \n",
                                i);

                    // Recalculate the steps to take. Note that by reducing
                    // stepTotal, this can break the while loop.
                    stepTotal = max(
                        abs(velFinish.x),
                        abs(velFinish.y +
                            gravity / 2)); // Note: A linear approximation of a
                                           // real trajectory under gravity.
                    stepX = velFinish.x / double(stepTotal);
                    stepY = (velFinish.y + gravity / 2) / double(stepTotal);

                    ULstartOrBounce = ULlast;
                    stepCountSinceBounce = 0;

                    break;
                }
            }
        } else {
            stepCount++;
            stepCountSinceBounce++;
            ULlast =
                ULnext; // The move to ULnext is valid, so we'll make it final.
        }
    }

    o->moveObjectToPoint(ULlast);

    velFinish.y += gravity;
    o->adjustVelocity(velFinish);

    return 1;
}
