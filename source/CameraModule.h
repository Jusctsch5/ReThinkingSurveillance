#ifndef CAMERAMODULE_H_
#define CAMERAMODULE_H_

#include "AbstractModule.h"

class CameraModule : public AbstractModule {
public:
    CameraModule();

    int init(void) override;
    int process(void) override;

private:
};

#endif // CAMERAMODULE_H_
