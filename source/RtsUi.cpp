#include "RtsUi.h"
#include "AbstractModule.h"
#include "PhysicsBoundary.h"
#include "RtsObjectList.h"

#define JU_OFFSET 20
#define JI_OFFSET 2

int d;
int total_objects = 0;
int current_module_g = MAIN;

using namespace std;
cv::Point *points[2] = {0, 0}, *swap_points;

cv::Mat *image = 0, *grey = 0, *prev_grey = 0, *pyramid = 0, *prev_pyramid = 0,
        *swap_temp, *map_image = 0;

int win_size = 10;
const int MAX_COUNT = 500;

cv::Point *temp_point[1] = {0};
char *status = 0;
int flags = 0;
RtsObjectList *ObjectList_l;

int toggle = 1; // for PAINT
int color = 0;  // for PAINT
PhysicsBoundary fence =
    PhysicsBoundary(BoundRect(cv::Point(15, 15), cv::Point(600, 460), 15), 3);

// Camera stuff
cv::Point cameraOffset;
cv::Point cameraOffsetTotal;
int camera_iteration = 0;
int camera_cooldown = 0;

RtsUi::RtsUi()
    : capture(0), mainModule(), cameraModule(), mapModule(), paintModule(),
      physicsModule(), rCurrentModule(mainModule), modules(), flowDetector() {
    modules.push_back(mainModule);
    modules.push_back(cameraModule);
    modules.push_back(mapModule);
    modules.push_back(paintModule);
    modules.push_back(physicsModule);
}

int RtsUi::launchUI(void) {

    int err = initializeObjects();
    if (err) {
        cerr << "initializeObjects error:" << err;
        return err;
    }

    if (!capture.isOpened()) {
        fprintf(stderr, "Could not initialize capturing\n");
        return -1;
    }

    // Print a welcome message, and the OpenCV version
    cv::namedWindow("RTS", 0);
    cv::namedWindow("map", 0);

    for (;;) {
        err = processInput();
        if (err) {
            cerr << "ProcessInput() error:" << err;
            return err;
        }

        capture.release();
        // video.release();
        cv::destroyAllWindows();

        return 0;
    }
}

int RtsUi::initializeObjects(void) {
    // create Objects

    RtsObjectList allList;
    allList.pushObject(*new RtsObject(cv::imread("imgs/mainslider.png"),
                                      BACKGROUND, cv::Point(GetHalfX(), 40),
                                      ALL, 0));
    allList.pushObject(*new RtsObject(cv::imread("imgs/SliderIcon.png"),
                                      NOY_IMAGE, cv::Point(GetHalfX(), 40), ALL,
                                      0));
    allList.pushObject(
        *new RtsObject(cv::imread("imgs/Chooser.png"), BACKGROUND,
                       cv::Point(GetHalfX(), GetHalfY()), CHOOSER, 0));
    allList.pushObject(*new RtsObject(
        cv::imread("imgs/Camera_s.png"), IMAGE,
        cv::Point(GetHalfX(), abs(GetHalfY() - 100)), CHOOSER, 0));

    RtsObjectList mainList;
    mainList.pushObject(*new RtsObject(cv::imread("imgs/MainBackground.png"),
                                       BACKGROUND, cv::Point(320, 240), MAIN,
                                       0));

    RtsObjectList paintList;
    paintList.pushObject(*new RtsObject(cv::imread("imgs/binary_ud_slider.png"),
                                        BACKGROUND, cv::Point(600, 150), PAINT,
                                        0));
    paintList.pushObject(*new RtsObject(cv::imread("imgs/SliderIcon_ud.png"),
                                        NOX_IMAGE, cv::Point(600, 150), PAINT,
                                        0));
    paintList.pushObject(*new RtsObject(cv::imread("imgs/binary_ud_slider.png"),
                                        BACKGROUND, cv::Point(30, 220), PAINT,
                                        0));
    paintList.pushObject(*new RtsObject(cv::imread("imgs/SliderIcon_ud.png"),
                                        NOX_IMAGE, cv::Point(30, 225), PAINT,
                                        1));

    RtsObjectList cameraList;
    cameraList.pushObject(*new RtsObject(cv::imread("imgs/compass.png"), IMAGE,
                                         cv::Point(GetHalfX(), GetHalfY()),
                                         CAMERA, 0));
    cameraList.pushObject(
        *new RtsObject(cv::imread("imgs/binary_ud_slider.png"), BACKGROUND,
                       cv::Point(610, 150), CAMERA, 0));
    cameraList.pushObject(*new RtsObject(cv::imread("imgs/SliderIcon_ud.png"),
                                         NOX_IMAGE, cv::Point(610, 150), CAMERA,
                                         0));

    RtsObjectList mapList;
    mapList.pushObject(*new RtsObject(cv::imread("imgs/mapmap.png"), BACKGROUND,
                                      cv::Point(GetHalfX(), GetHalfY()), MAP,
                                      0));
    mapList.pushObject(*new RtsObject(cv::imread("imgs/x.png"), IMAGE,
                                      cv::Point(GetHalfX(), GetHalfY()), MAP,
                                      0));

    RtsObjectList physicsList;
    physicsList.pushObject(
        *new RtsObject(cv::imread("imgs/physicsback.png"), BACKGROUND,
                       cv::Point(GetHalfX(), GetHalfY()), PHYSICS, 0));
    physicsList.pushObject(*new RtsObject(cv::imread("imgs/rball.png"), IMAGE,
                                          cv::Point(100, 100), PHYSICS, 0));
    physicsList.pushObject(*new RtsObject(cv::imread("imgs/yball.png"), IMAGE,
                                          cv::Point(100, 150), PHYSICS, 1));
    physicsList.pushObject(*new RtsObject(cv::imread("imgs/gball.png"), IMAGE,
                                          cv::Point(100, 250), PHYSICS, 2));

    mainModule.addObjectList(allList);
    mainModule.addObjectList(mainList);

    cameraModule.addObjectList(allList);
    cameraModule.addObjectList(cameraList);

    mapModule.addObjectList(allList);
    mapModule.addObjectList(mapList);

    paintModule.addObjectList(allList);
    paintModule.addObjectList(paintList);

    physicsModule.addObjectList(allList);
    physicsModule.addObjectList(physicsList);

    for (auto &rModule : modules) {
        rModule.init();
    }

    return 0;
}

int RtsUi::processInput(void) {
    cv::Mat frame;

    int i = 0;
    int c = 0;

    capture >> frame;
    if (frame.empty()) {
        fprintf(stderr, "Could not get a new frame\n");
        return -1;
    }

    // cvFlip(frame, NONE, 1);

    // first pass
    /*
    if (image == NULL) {
        // allocate all the buffers
        image = cv::Mat(cv::getSize(frame), CV_8UC3);
        map_image = cv::Mat(cv::getSize(frame), CV_8UC3);
        map_image->origin = frame->origin;
        image->origin = frame->origin;
        grey = cv::Mat(cv::getSize(frame), CV_8UC1);
        prev_grey = cv::Mat(cv::getSize(frame), CV_8UC1);
        pyramid = cv::Mat(cv::getSize(frame), CV_8UC1);
        prev_pyramid = cv::Mat(cv::getSize(frame), CV_8UC1);
        points[0] =
            (cv::Point2D32f *)cvAlloc(MAX_OBJECTS * sizeof(points[0][0]));
        points[1] =
            (cv::Point2D32f *)cvAlloc(MAX_OBJECTS * sizeof(points[0][0]));
        status = (char *)cvAlloc(MAX_OBJECTS);
        flags = 0;
    }
    */

    list<RtsObject>::iterator it;

    flowDetector.configureTracking();

    std::vector<cv::Point> flowPoints;
    flowDetector.trackNextFrame(frame, flowPoints);
    rCurrentModule.process();

    /*
    for (it = ObjectList.begin(); it != ObjectList.end(); it++, i++) {
        if ((it->module != ALL) && (it->module != current_module_g)) {
            points[1][i] = points[0][i];
            continue;
        }
        if (it->type == BACKGROUND)
            continue;
        if (it->module == PAINT) {
            if (it->type == IMAGE) {
                cameraOffset.x =
                    cv::Point(points[1][i]).x - cv::Point(points[0][i]).x;
                cameraOffset.y =
                    cv::Point(points[1][i]).y - cv::Point(points[0][i]).y;
                points[1][i] = cv::Point(cv::Point(GetHalfX(), GetHalfY()));
                continue;
            }
        }
        if (!status[i]) {
            points[1][i] = points[0][i];
            // if (ObjectList->Array[i]->type == NOY_IMAGE)
            // points[1][i].y =40; use the OLD LOCATION!!! don't just
            // ignore
            continue;
        }
        // fprintf(stderr,"\t\tX
        // value:%d\n",cv::Point(points[1][i]).x);
        // fprintf(stderr,"\t\tY
        // value:%d\n",cv::Point(points[1][i]).y);
        removeJump(i);
        //  removeJitter(i);
        if (it->type == NOY_IMAGE) {
            // assume main slider for now
            points[1][i].y = points[0][i].y;
        } else
            it->center.y = cv::Point(points[1][i]).y;
        if (it->type == NOX_IMAGE) {
            points[1][i].x = points[0][i].x;
        } else
            it->center.x = cv::Point(points[1][i]).x;
    }

    while (!objectQueue.empty()) {
        ObjectList.push_back(objectQueue.front());
        points[1][total_objects] = cv::Point(objectQueue.front().center);
        objectQueue.pop();
        total_objects++;
    }
    */

    switch (current_module_g) {
    case MAIN:
        break;
    case CHOOSER:
        handleChooser();
        break;
    case PAINT:
        handlePaint();
        mt(image, capture, toggle, color);
        break;
    case PHYSICS:
        handlePhysics();
        break;
    case CAMERA:
        handleCamera();
        break;
    }

    // render the relevant objects
    handleObjects(image);
    if (current_module_g == MAP)
        handleMap();

    // update old images for motion tracking
    CV_SWAP(prev_grey, grey, swap_temp);
    CV_SWAP(prev_pyramid, pyramid, swap_temp);
    CV_SWAP(points[0], points[1], swap_points);

    cvShowImage("RTS", image);
    cvShowImage("map", map_image);
    c = cv::waitKey(10);
    if ((char)c == 27)
        break;
}

void RtsUi::removeJump(int i) {
    cv::Point jumpOffset = {JU_OFFSET, JU_OFFSET};
    if (cv::Point(points[1][i]).x > cv::Point(points[0][i]).x + JU_OFFSET) {
        points[1][i].x = points[0][i].x + jumpOffset.x;
    } else if (cv::Point(points[1][i]).x <
               cv::Point(points[0][i]).x - JU_OFFSET) {
        points[1][i].x = points[0][i].x - jumpOffset.x;
    }
    if (cv::Point(points[1][i]).y > cv::Point(points[0][i]).y + JU_OFFSET) {
        points[1][i].y = points[0][i].y + jumpOffset.y;
    } else if (cv::Point(points[1][i]).y <
               cv::Point(points[0][i]).y - JU_OFFSET) {
        points[1][i].y = points[0][i].y - jumpOffset.y;
    }
}

void RtsUi::removeJitter(int i) {
    cv::Point JitterOffset = {JI_OFFSET, JI_OFFSET};
    if (cv::Point(points[1][i]).x <= cv::Point(points[0][i]).x + JI_OFFSET &&
        cv::Point(points[1][i]).x > cv::Point(points[0][i]).x) {
        points[1][i].x = points[0][i].x;
    } else if (cv::Point(points[1][i]).x >=
                   cv::Point(points[0][i]).x - JI_OFFSET &&
               cv::Point(points[1][i]).x < cv::Point(points[0][i]).x) {
        points[1][i].x = points[0][i].x;
    }
    if (cv::Point(points[1][i]).y <= cv::Point(points[0][i]).y + JI_OFFSET &&
        cv::Point(points[1][i]).y > cv::Point(points[0][i]).y) {
        points[1][i].y = points[0][i].y;
    } else if (cv::Point(points[1][i]).y <=
                   cv::Point(points[0][i]).y - JI_OFFSET &&
               cv::Point(points[1][i]).y < cv::Point(points[0][i]).y) {
        points[1][i].y = points[0][i].y;
    }
}

void RtsUi::handleObjects(cv::Mat *img, RtsObjectList &irObjectList) {

    int i = 0;

    /* need to handle sliders */
    /*
                // for the main slider
            if (it->type == NOY_IMAGE && it->id == 0) {
                if (current_module_g == MAIN) {
                    if (it->center.x > 500) {
                        it->center.x = 500;
                        cv::Point(points[0][i]).x = 500;
                        cv::Point(points[1][i]).x = 500;
                        current_module_g = CHOOSER;
                        resetCursor();
                    }

                } else if (it->center.x < 100) {
                    it->center.x = 100;
                    cv::Point(points[0][i]).x = 100;
                    cv::Point(points[1][i]).x = 100;
                    current_module_g = MAIN;
                }
    */
    for (it = irObjectList.begin(); it != irObjectList.end(); it++, i++) {
        if ((it->module == current_module_g) || (it->module == ALL)) {
            // renders every relevant object for a given module
        }
        it->renderObject(image);
        cv::Point(points[1][i]) = it->center;
    }
}
}

void RtsUi::handlePhysics(void) {

    // if you want to handle physics in modules outside of physics, then
    // generalize this code aka throw it into the main loop
    int i = 0;
    list<RtsObject>::iterator it;
    for (it = ObjectList.begin(); it != ObjectList.end(); it++, i++) {
        if (it->type == NOY_IMAGE) {
            if (it->center.x > 500) {
                it->center.x = 500;
                /*
                                    for (ci=0; ci<ObjectList->total_size; ci++)
                                    {
                                        if (ObjectList->Array[ci]->type == IMAGE
                   && ObjectList->Array[ci]->module == IMAGE)
                                        {
                                             ObjectList->Array[ci]->Center.x =
                   getRandX(); ObjectList->Array[ci]->Center.y = getRandY();
                                        }
                                    }
                                    */
            }
        }
        if (it->module != PHYSICS)
            continue;
        RtsObject *temp = new RtsObject(0, 0, cv::Point(320, 240), 0, 0);
        // temp->UL = it->UL;
        // temp->BR = it->BR;

        // it.assign(temp);
        moveObject(fence, it);
        points[1][i] = cv::Point(it->center);
    }
}

void RtsUi::handlePaint(void) {
    int i = 0;
    int old_color = color;
    list<RtsObject>::iterator it;
    for (it = ObjectList.begin(); it != ObjectList.end(); it++, i++) {
        if ((it->type == NOX_IMAGE) && (it->module == PAINT)) {
            // color slider
            if (it->id == 1) {
                if (it->center.y < 160) {
                    it->center.y = 160;
                    points[1][i].y = cv::Point(it->center).y;
                }
                color = 225 - cv::Point(points[0][i]).y;
                if (it->center.y > 300) // finetune these values
                {
                    it->center.y = 300;
                    points[1][i].y = cv::Point(it->center).y;
                    color = 0;
                }
                // going to be from 0 to 24
            }

            // toggle slider
            if (it->id == 0) {
                if (it->center.y > 225) {
                    it->center.y = 225;

                    points[1][i].y = cv::Point(it->center).y;
                    toggle = 1;
                }
                if (it->center.y < 80) {
                    it->center.y = 80;
                    points[1][i].y = cv::Point(it->center).y;
                    toggle = 0;
                }
            }
        }
    }
}

/*
___
|  |
|__|
Get it in the box
Ex
UL 263,345
Center 300,371
BR     337,395
for MAP

37 and 25?
*/
void RtsUi::handleChooser(void) {
    // for our project

    list<RtsObject>::iterator it;
    for (it = ObjectList.begin(); it != ObjectList.end(); it++) {
        if (it->module != CHOOSER)
            continue;
        if (it->type != IMAGE)
            continue;
        // select the chooser icon
        break;
    }

    int x = it->center.x;
    int y = it->center.y;
    fprintf(stderr, "__________________%d\n", x);
    fprintf(stderr, "__________________%d\n", y);
    // Maps
    if ((x > 263) && (x < 337) && (y > 345) && (y < 395))
        current_module_g = MAP;
    else if ((x > 126) && (x < 200) && (y > 271) && (y < 321))
        current_module_g = CAMERA;
    else if ((x > 408) && (x < 482) && (y > 266) && (y < 316))
        current_module_g = PHYSICS;
    else if ((x > 490) && (x < 564) && (y > 148) && (y < 198))
        current_module_g = PAINT;
}

void RtsUi::handleMap(void) {
    list<RtsObject>::iterator it;
    for (it = ObjectList.begin(); it != ObjectList.end(); it++) {
        if (it->module != MAP)
            continue;
        if (it->type != IMAGE)
            continue;
        // select the map cursor
        break;
    }
    checkLocation(cv::Point(204, 370), it->center, image, map_image);
}

void RtsUi::handleCamera(void) {
    int CameraZoom = 0;
    list<RtsObject>::iterator it;
    int i = 0;
    for (it = ObjectList.begin(); it != ObjectList.end(); it++, i++) {
        if (it->module == CAMERA) {
            if (it->type == NOX_IMAGE) {
                if (cv::Point(points[1][i]).y > 225) {
                    it->center.y = 225;
                    points[1][i] = cv::Point(it->center);
                }
                CameraZoom = cv::Point(points[1][i]).y;
            }
        }
    }
    if (camera_iteration == 0) {
        cameraOffsetTotal.x = 0;
        cameraOffsetTotal.y = 0;
    }
    if (((abs(cameraOffset.x) > 6) || (abs(cameraOffset.y) > 6)) &&
        (!camera_cooldown)) {
        fprintf(stderr, "*******cameraOffset_x:%d\n", cameraOffset.x);
        fprintf(stderr, "*******cameraOffset_y:%d\n", cameraOffset.y);
        fprintf(stderr, "********CameraZoom:%d\n", CameraZoom);
        camera_cooldown = 15;
        // update camera
    }

    if (camera_cooldown)
        camera_cooldown--;
    camera_iteration++;
}

void RtsUi::resetCursor(void) {
    int i = 0;
    list<RtsObject>::iterator it;
    for (it = ObjectList.begin(); it != ObjectList.end(); it++, i++) {
        if (it->module != CHOOSER)
            continue;
        if (it->type != IMAGE)
            continue;
        it->center = cv::Point(GetHalfX(), GetHalfY() - 100);
        points[1][i] = cv::Point(it->center);
        points[0][i] = cv::Point(it->center);
        // select the chooser icon
        break;
    }
}
