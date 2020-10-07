# %Tag(FULLTEXT)%
cmake_minimum_required(VERSION 2.8.3)
project(beginner_tutorials)
set(raspicam_DIR "/usr/local/lib/cmake")
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.0)
pkg_check_modules(GTKMM glibmm-2.4)

include_directories(/usr/local/include)
INCLUDE_DIRECTORIES(${GTKMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLIBMM_INCLUDE_DIRS})

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg)
find_package(OpenCV)
find_package(raspicam REQUIRED)

LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})
LINK_DIRECTORIES(${GLIBMM_LIBRARY_DIRS})

## Declare a catkin package
catkin_package()

## Build talker and listener
include_directories(include ${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")
ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()

add_executable(principal src/principal.cpp)
target_link_libraries(principal ${catkin_LIBRARIES} wiringPi)
add_dependencies(principal beginner_tutorials_generate_messages_cpp)

add_executable(status src/status.cpp)
target_link_libraries(status ${catkin_LIBRARIES})
add_dependencies(status beginner_tutorials_generate_messages_cpp)

add_executable(botones src/botones.cpp)
target_link_libraries(botones ${catkin_LIBRARIES} wiringPi)
add_dependencies(botones beginner_tutorials_generate_messages_cpp)

##add_executable(matriz src/matriz.cpp)
##target_link_libraries(matriz ${catkin_LIBRARIES} wiringPi ${GLIBMM_LIBRARIES} ${GTKMM_LIBRARIES})
##add_dependencies(matriz beginner_tutorials_generate_messages_cpp)
add_definitions(-std=c++11)
add_executable(vision src/vision.cpp)
target_link_libraries(vision ${catkin_LIBRARIES} ${raspicam_CV_LIBS} ${OpenCV_LIBS})
add_dependencies(vision beginner_tutorials_generate_messages_cpp)


# %EndTag(FULLTEXT)%