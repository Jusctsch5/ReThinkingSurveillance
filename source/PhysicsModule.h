#ifndef PHYSICSMODULE_H_
#define PHYSICSMODULE_H_

#include "AbstractModule.h"

class PhysicsModule : public AbstractModule {
public:
    PhysicsModule();

    int init(void) override;
    int process(void) override;

private:
};

#endif // PHYSICSMODULE_H_
