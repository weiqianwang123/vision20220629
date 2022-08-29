cmake_minimum_required(VERSION 3.22)
project(20220629)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV 4 REQUIRED)
find_package( Pangolin )
message(STATUS "OpenCVss library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")
    # Add OpenCV headers location to your include paths
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

INCLUDE_DIRECTORIES("include/")
INCLUDE_DIRECTORIES(
        "include/"
        "include/mercure")
INCLUDE_DIRECTORIES("/usr/include/eigen3")
#INCLUDE_DIRECTORIES("/home/wang/文档/slambook2/3rdparty/Pangolin)
include_directories( ${Pangolin_INCLUDE_DIRS} )
add_executable(20220629 main.cpp cmake-build-debug include/Armor_Detector.h src/Armor_Detector.cpp src/ArmorBox.cpp include/ArmorBox.h include/const.h include/Solver.h src/Solver.cpp include/ImageProgess.h src/ImageProgess.cpp)
add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mercure
)
#target_link_libraries(20220629 -I/usr/local/include/opencv -I/usr/local/include -I/usr/local/lib -lopencv_dnn -lopencv_ml -lopencv_objdetect -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core)

target_link_libraries(20220629 mercure_driver  ${OpenCV_LIBS})
