// PhysicsModule.cpp

#include "PhysicsModule.h"
#include <random>

PhysicsModule::PhysicsModule() : AbstractModule() {}

int PhysicsModule::init() { return 0; }
int PhysicsModule::process() { return 0; }
int getRandX(void) { return rand() % 630 + 10; }
int getRandY(void) { return rand() % 100 + 10; }
