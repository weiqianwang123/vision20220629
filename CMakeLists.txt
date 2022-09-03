cmake_minimum_required(VERSION 3.10)
project(20220629)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV 4 REQUIRED)
#find_package( Pangolin )
message(STATUS "OpenCVss library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPATH=\"\\\"${PROJECT_SOURCE_DIR}\\\"\"")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CMAKE_SYSTEM_NAME}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

INCLUDE_DIRECTORIES(
        "include/"
        "include/mercure"
        "/usr/include/eigen3")

FILE(GLOB_RECURSE sourcefiles "src/*.cpp")

add_executable(20220629 main.cpp ${sourcefiles})
add_subdirectory(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mercure
)

target_link_libraries(20220629 mercure_driver ${OpenCV_LIBS} )
