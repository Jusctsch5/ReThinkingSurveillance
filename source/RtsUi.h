#ifndef RTSUI_H_
#define RTSUI_H_

#include "CameraModule.h"
#include "CvFlowDetector.h"
#include "MainModule.h"
#include "MapModule.h"
#include "PaintModule.h"
#include "PhysicsModule.h"
#include "RtsCommon.h"


#include <algorithm>
#include <ctype.h>
#include <list>
#include <queue>
#include <stdio.h>
#include <string>

#include <opencv2/opencv.hpp>

#define X_SCREEN_MIN 0
#define Y_SCREEN_MIN 0
#define X_SCREEN_MAX 640
#define Y_SCREEN_MAX 480

#define H 480
#define W 640

class AbstractModule;

class RtsUi {
public:
    RtsUi();
    int launchUI(void);

    static int GetMinX(void) { return minX; }
    static int GetMaxX(void) { return maxX; }
    static int GetHalfX(void) { return maxX / 2; }
    static int GetMinY(void) { return minY; }
    static int GetMaxY(void) { return maxY; }
    static int GetHalfY(void) { return maxY / 2; }

private:
    int initializeObjects(void);
    int processInput(void);
    void removeJump(int i);
    void removeJitter(int i);
    void initStuff(void);
    void menuUnlocked(void);
    void handlePaint(void);
    void handlePhysics(void);
    void handleChooser(void);
    void handleMap(void);
    void handleObjects(cv::Mat *img);
    void checkHSlider(void);
    void checkVSlider(void);
    void resetCursor(void);
    void handleCamera(void);

    cv::VideoCapture capture;

    MainModule mainModule;
    CameraModule cameraModule;
    MapModule mapModule;
    PaintModule paintModule;
    PhysicsModule physicsModule;
    AbstractModule &rCurrentModule;
    std::list<AbstractModule> modules;
    CvFlowDetector flowDetector;

    static int minX;
    static int maxX;
    static int minY;
    static int maxY;
};

#endif // RTSUI_H_
