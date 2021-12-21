#ifndef MAINMODULE_H_
#define MAINMODULE_H_

#include "AbstractModule.h"

class MainModule : public AbstractModule {
public:
    MainModule();

    int init(void) override;
    int process(void) override;

private:
};

#endif // MAINMODULE_H_
