#####################################
cmake_minimum_required (VERSION 2.8) 
project (rasp)
set(raspicam_DIR "/usr/local/lib/cmake")
find_package(PkgConfig)
add_definitions(-std=c++11)
pkg_check_modules(GTKMM gtkmm-2.0)
pkg_check_modules(GTKMM glibmm-2.4)

include_directories (/home/pi/tesseract)
include_directories(./leptonica-1.74.1/src)
include_directories(./leptonica-1.74.1)
include_directories(/usr/local/include)
INCLUDE_DIRECTORIES(${GTKMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GLIBMM_INCLUDE_DIRS})

find_library(ESTOOLS_LIBRARY estools /home/pi/speech_tools/lib)
find_library(ESTSTRING_LIBRARY eststring /home/pi/speech_tools/lib)
find_library(ESTBASE_LIBRARY estbase /home/pi/speech_tools/lib)
find_library(Festival_LIBRARIES Festival /home/pi/festival/src/lib)
find_library(wiringPi_LIBRARIES wiringPi /usr/local/lib)

LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})
LINK_DIRECTORIES(${GLIBMM_LIBRARY_DIRS})

set(EST_LIBRARIES
	${ESTOOLS_LIBRARY}
	${ESTSTRING_LIBRARY}
	${ESTBASE_LIBRARY}
)

find_path(Festival_INCLUDE_DIR festival.h /home/pi/festival/src/include)
find_path(EST_INCLUDE_DIR EST.h /home/pi/speech_tools/include)


find_package(raspicam REQUIRED)
find_package(OpenCV)

INCLUDE_DIRECTORIES(include	${EST_INCLUDE_DIR}
				${Festival_INCLUDE_DIR})



link_directories(/home/pi/tesseract)
IF  ( OpenCV_FOUND AND raspicam_CV_FOUND)
MESSAGE(STATUS "COMPILING OPENCV TESTS")
add_executable (rasp main.cpp)  
target_link_libraries (rasp ${raspicam_CV_LIBS} wiringPi lept tesseract ${EST_LIBRARIES} ${Festival_LIBRARIES} ${GLIBMM_LIBRARIES} ${GTKMM_LIBRARIES})
ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()
#####################################