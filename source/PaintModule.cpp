
#include "PaintModule.h"

// various tracking parameters (in seconds)
const double MHI_DURATION = 3;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;

// number of cyclic frame buffer used for motion detection
// (should, probably, depend on FPS)
const int N = 4;

int color_counter = 0;
int toggle_b = 1; // change this for the background *use slider?*

// ring image buffer
cv::Mat **buf = 0;
int last = 0;

// temporary images

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
PaintModule::PaintModule()
    : AbstractModule(), previousMHI(NULL), orient(NULL), mask(NULL),
      segmask(NULL) {}

int PaintModule::init() { return 0; }
int PaintModule::process() { return 0; }

int PaintModule::updateMotionHistoryImage(cv::Mat *img, cv::Mat *dst,
                                          int diff_threshold) {
    double timestamp =
        (double)clock() / CLOCKS_PER_SEC; // get current time in seconds
    cv::Size size = cv::Size(img->size().width,
                             img->size().height); // get current frame size
    int i, idx1 = last, idx2;
    cv::Mat *silh;

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if (!previousMHI || previousMHI->size().width != size.width ||
        previousMHI->size().height != size.height) {
        if (buf == 0) {
            buf = (cv::Mat **)malloc(N * sizeof(buf[0]));
            // printf("successfully malloced %d bytes\n", N*sizeof(buf[0]));
            memset(buf, 0, N * sizeof(buf[0]));
        }

        for (i = 0; i < N; i++) {
            buf[i]->release();
            buf[i] = cv::Mat(size, CV_8UC1) cv::zero(buf[i]);
        }
        previousMHI->release();
        orient->release();
        segmask->release();
        mask->release();
        previousMHI = cv::createImage(size, IPL_DEPTH_32F, 1);
        cv::zero(previousMHI); // clear MHI at the beginning
        orient = cv::createImage(size, IPL_DEPTH_32F, 1);
        segmask = cv::createImage(size, IPL_DEPTH_32F, 1);
        mask = cv::createImage(size, IPL_DEPTH_8U, 1);
    }

    cvCvtColor(img, buf[last], CV_BGR2GRAY); // convert frame to grayscale

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    if (toggle_b)
        cvAbsDiff(buf[idx1], buf[idx2], silh); // get difference between frames

    cvThreshold(silh, silh, diff_threshold, 1,
                CV_THRESH_BINARY); // and threshold it
    cvUpdateMotionHistory(silh, previousMHI, timestamp,
                          MHI_DURATION); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale(previousMHI, mask, 255. / MHI_DURATION,
               (MHI_DURATION - timestamp) * 255. / MHI_DURATION);
    cv::zero(dst);
    // cvCvtPlaneToPix(mask, 0, 0, 0, dst);
    if (color_counter < 1000) {
        cvMerge(mask, 0, 0, 0, dst);
    } else if (color_counter < 2000) {
        cvMerge(0, mask, 0, 0, dst);
    } else if (color_counter < 3000) {
        cvMerge(0, 0, mask, 0, dst);
    } else if (color_counter < 4000) {
        cvMerge(mask, mask, 0, 0, dst);
    } else if (color_counter < 5000) {
        cvMerge(mask, 0, mask, 0, dst);
    } else if (color_counter < 6000) {
        cvMerge(mask, mask, mask, 0, dst);
    } else {
        color_counter = 0;
    }

    if (!storage) {
        storage = cvCreateMemStorage(0);
    } else {
        cvClearMemStorage(storage);
    }

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further

    return 0;
}

int PaintModule::trackMotion(cv::Mat *image, CvCapture *capture, int t, int c) {
    cv::Mat *motion = 0;
    color_counter += c;
    if (!motion) {
        motion = cv::createImage(
            cv::Size(image->size().width, image->size().height), 8, 3);
        cv::zero(motion);
        motion->origin = image->origin;
    }
    /*
    toggle++;
    if (toggle == 2) toggle = 0;
    */
    toggle_b = t;
    int ret = updateMotionHistoryImage(image, motion, 30);
    if (ret != 0) {
        return ret;
    }

    cv::copy(motion, image, 0);
    return 0;
}
