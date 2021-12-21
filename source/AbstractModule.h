#ifndef ABSTRACTMODULE_H_
#define ABSTRACTMODULE_H_

#include "RtsObjectList.h"

class AbstractModule {
public:
    AbstractModule() : objects() {}
    /* Describes one-time actions for the module to perform, like memory
     * allocations */
    virtual int init(void) = 0;

    /* Called per frame */
    virtual int process(void) = 0;

    void addObject(RtsObject &irObject) { objects.pushObject(irObject); }
    void addObjectList(RtsObjectList &irObjectList) {
        objects.pushObjects(irObjectList);
    }

private:
    RtsObjectList objects;
};

#endif // ABSTRACTMODULE_H_
