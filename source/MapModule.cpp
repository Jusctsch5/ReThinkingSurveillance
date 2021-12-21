// MapModule.c
#include "MapModule.h"

using namespace std;

MapModule::MapModule(void)
    : AbstractModule(), locations(), savedCurrentLocation(),
      locationFound(false) {}

int MapModule::init(void) {
    initLocations();
    return 0;
}

int MapModule::process(void) { return 0; }

void MapModule::initLocations(void) {
    locations.push_front(*new RtsLocation(
        cv::Point(180, 89), cv::Point(462, 200), "Coffman Bookstore"));
    locations.push_front(*new RtsLocation(
        cv::Point(345, 273), cv::Point(450, 340), "Coffman Dining Center"));
    locations.push_front(*new RtsLocation(cv::Point(158, 250),
                                          cv::Point(280, 391), "Grand Hall"));
    locations.push_front(*new RtsLocation(cv::Point(285, 368),
                                          cv::Point(329, 405), "Exit Here!"));
}

void MapModule::checkLocation(cv::Point currentLocation,
                              cv::Point pointedLocation, cv::Mat *ui,
                              cv::Mat *map) {
    savedCurrentLocation = currentLocation;
    locationfound = 0;

    cv::line(*ui, currentLocation, pointedLocation, cv::Scalar(0, 0, 0, 0), 4,
             8, 0);
    list<RtsLocation>::iterator it;
    for (it = locations.begin(); it != locations.end(); it++) {
        if (it->pointExistsIn(pointedLocation)) {
            locationFound = 1;
            it->displayInfo(map, currentLocation);
        }
    }
    // no location highlighted, deselect
    if (!locationfound) {
        map = &cv::Scalar(0, 0, 0);
    }
}
