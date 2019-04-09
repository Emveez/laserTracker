cmake_minimum_required(VERSION 2.8)
project(laserTracker)

set (CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB sourceFiles
    "src/*.h"
    "src/*.cpp"
)

add_executable(laserTracker ${sourceFiles})

target_link_libraries(laserTracker ${OpenCV_LIBS})