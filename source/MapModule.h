#ifndef MAPMODULE_H_
#define MAPMODULE_H_

#include "AbstractModule.h"
#include "RtsLocation.h"
#include <list>
#include <math.h>
#include <stdlib.h>
#include <string>

cv::Point CurrentLocation;
int locationfound;

class MapModule : public AbstractModule {
public:
    MapModule(void);
    int init(void) override;
    int process(void) override;
    void initLocations(void);
    void checkLocation(cv::Point CL, cv::Point PL, cv::Mat *ui, cv::Mat *map);
    void AddObject(RtsObject &irObject);

private:
    std::list<RtsLocation> locations;
    cv::Point savedCurrentLocation;
    bool locationFound;
};

#endif // MAPMODULE_H_