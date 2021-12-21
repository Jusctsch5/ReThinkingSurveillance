#ifndef PAINTMODULE_H_
#define PAINTMODULE_H_

#include <opencv2/opencv.hpp>

#include "AbstractModule.h"

class PaintModule : public AbstractModule {
public:
    PaintModule();

    int init(void) override;
    int process(void) override;
    int trackMotion(void);
    int updateMotionHistoryImage(cv::Mat *img, cv::Mat *dst,
                                 int diff_threshold);

private:
    cv::Mat *previousMHI; // MHI
    cv::Mat *orient;      // orientation
    cv::Mat *mask;        // valid orientation mask
    cv::Mat *segmask;     // motion segmentation map
};

#endif // PAINTMODULE_H_