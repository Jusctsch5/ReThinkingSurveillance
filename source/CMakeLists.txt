cmake_minimum_required(VERSION 3.0.0)
project(rts VERSION 0.1.0)

include(CTest)
enable_testing()

find_package( OpenCV REQUIRED)
message( "Interesting CMake Variables")
message( "OpenCV IncludeDir: ${OpenCV_INCLUDE_DIRS}" )
message( "OpenCV Libs: ${OpenCV_LIBS}" )


include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(rts
main.cpp
RtsUi.cpp
RtsObject.cpp
RtsObjectList.cpp
RtsLocation.cpp
MainModule.cpp
CameraModule.cpp
MapModule.cpp
PaintModule.cpp
PhysicsBoundary.cpp
PhysicsModule.cpp
)

target_link_libraries( rts ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

