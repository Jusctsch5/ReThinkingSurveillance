// openCV OO classes
#include "RtsObject.h"
#include "RtsUi.h"

RtsObject::RtsObject(cv::Mat &irImage, ImageTypeEnum iImageType,
                     cv::Point init_center, ModuleTypeEnum iModuleType, int i) {
    if (iImageType != NONE) {
        this->id = i;
        this->module = iModuleType;
        this->rImage = irImage;
        this->type = iImageType;
        this->center = init_center;
        if (iImageType == IMAGE || iImageType == NOY_IMAGE ||
            iImageType == NOX_IMAGE) {
            this->motion = 1;
            this->gravity = 0;
            this->moving = 0;
            this->friction = 0;
            this->elasticity = 0;
        }

        if (iModuleType == PHYSICS) {
            this->motion = 1;
            this->gravity = 0;
            this->moving = 1;
            if (this->id == 0) {
                this->friction = 1.0;
                this->elasticity = 1.0;
                this->gravity = 0;
                this->velocity.x = 10;
                this->velocity.y = 10;
            }
            if (this->id == 1) {
                this->friction = .99;
                this->elasticity = .95;
            }
            if (this->id == 2) {
                this->friction = .99;
                this->elasticity = .99;
            }
            this->velocity.x = 10;
            this->velocity.y = 10;
            return;
        }

        if ((iImageType == SLIDER) || (iImageType == BACKGROUND)) {

            this->motion = 0;
            this->moving = 0;
            this->gravity = 0;
            this->friction = 0;
            this->elasticity = 0;
            this->velocity = cv::Point(0, 0);
        }
    }
}

void RtsObject::findUpperLeftPoint(cv::Point center) {
    this->upperLeft.x = center.x - (rImage.size().width) / 2;
    this->upperLeft.y = center.y - (rImage.size().height) / 2;
}
void RtsObject::findCenterPoint(void) {
    this->center.x = this->upperLeft.x + (rImage.size().width) / 2;
    this->center.y = this->upperLeft.y + (rImage.size().height) / 2;
}

void RtsObject::findBottomRightPoint(cv::Point center) {
    this->bottomRight.x = center.x + (rImage.size().width) / 2;
    this->bottomRight.y = center.y + (rImage.size().height) / 2;
}

void RtsObject::renderObject(cv::Mat *background) {
    rImage.copyTo(*background);

    /*
    int i;
    int j;
    // fprintf(stderr,"OO Blending:");
    // fprintf(stderr,"\tCenter %d,%d",this->Center.x,this->Center.y);
    this->checkBounds();
    findUpperLeftPoint(this->center);
    findBottomRightPoint(this->center);

    // find center for displaying
    background =
        cvSetImageROI(, cv::Rect(this->upperLeft.x, this->upperLeft.y,
                                 rImage.size().width, rImage.size().height));
    // do pixel by pixel
    // cvAddWeighted(blendimage, alpha, image, beta, 0.0,image); //not needed
    for (i = 0; i < rImage.size().height; i++) {
        uchar *ptr_background =
            (uchar *)(background->imageData +
                      (i + upperLeft.y) * background->widthStep);
        uchar *ptr_image =
            (uchar *)(rImage.imageData + i * rImage.size().widthStep);

        for (j = 0; j < rImage.size().width; j++) {
            if (ptr_image[3 * j + 0] != 255 || ptr_image[3 * j + 1] != 255 ||
                ptr_image[3 * j + 2] != 255) {
                ptr_background[3 * (j + upperLeft.x) + 0] =
                    ptr_image[3 * (j) + 0];
                ptr_background[3 * (j + upperLeft.x) + 1] =
                    ptr_image[3 * (j) + 1];
                ptr_background[3 * (j + upperLeft.x) + 2] =
                    ptr_image[3 * (j) + 2];
            }
        }
    }
    // fprintf(stderr,"\n Done OOrender\n");
    background = cvResetImageROI(background);
    // end renderObject
    */
}

void RtsObject::checkBounds(void) {
    if (this->type == BACKGROUND)
        return; // assume they set correct coordinates
    findUpperLeftPoint(this->center);
    findBottomRightPoint(this->center);
    // fprintf(stderr,"\n\t UL: %d,%d",UL.x,UL.y);
    // fprintf(stderr," BR: %d,%d",BR.x,BR.y);

    // check boundry
    if (upperLeft.y < RtsUi::GetMinY()) {
        upperLeft.y = 5;
        bottomRight.y = 5 + rImage.size().height;
    }

    if (upperLeft.x < RtsUi::GetMinX()) {
        upperLeft.x = 5;
        bottomRight.x = 5 + rImage.size().width;
    }

    if (bottomRight.y > RtsUi::GetMinY()) {
        upperLeft.y = RtsUi::GetMinY() - 10 - rImage.size().height;
    }

    if (bottomRight.x > RtsUi::GetMaxX()) {
        upperLeft.x = RtsUi::GetMaxX() - 10 - rImage.size().width;
    }

    // fprintf(stderr,"\n\t Adjusted UL!: %d,%d",UL.x,UL.y);
    // fprintf(stderr," Adjusted BR!: %d,%d",BR.x,BR.y);
    // reset center
    this->setCenterPoint();
}

void RtsObject::setCenterPoint(void) {
    this->center.x = this->upperLeft.x + (int)rImage.size().width * .5;
    this->center.y = this->upperLeft.y + (int)rImage.size().height * .5;
}

int RtsObject::applyFlow(std::vector<cv::Point> &irFlowPoints) {
    for (auto &rPoint : irFlowPoints) {
        // is this object or center point of object in the motion
        // if so, apply the motion to the object
    }
}

int RtsObject::moveObjectToPoint(cv::Point iUpperLeftPoint) {
    auto tempPoint = upperLeft;

    upperLeft.x = iUpperLeftPoint.x;
    upperLeft.y = iUpperLeftPoint.y;
    bottomRight.x = tempPoint.x + rImage.size().width;
    bottomRight.y = tempPoint.y + rImage.size().height;
    findCenterPoint();

    fprintf(stderr, "\tN: UL BR: %d,%d %d,%d\n", GetUpperLeft().x,
            GetUpperLeft().y, GetBottomRight().x, GetBottomRight().y);
}

int RtsObject::adjustVelocity(cv::Point iVelocity) {
    velocity.y = iVelocity.y;
    velocity.x = iVelocity.x;

    fprintf(stderr, "\t\t End Velocity x,y: %d,%d\n", velocity.x, velocity.y);
}
