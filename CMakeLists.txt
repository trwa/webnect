cmake_minimum_required(VERSION 3.31)
project(kinect)

set(CMAKE_CXX_STANDARD 23)

add_executable(kinect)

target_sources(kinect PUBLIC main.cpp KinectV1.cpp)
#target_sources(kinect PUBLIC FILE_SET modules TYPE CXX_MODULES FILES AtomicBuffer.cpp KinectV1.cppm)

target_include_directories(kinect PRIVATE /usr/include/libfreenect /usr/include/opencv4)
target_link_libraries(kinect PRIVATE atomic freenect opencv_core opencv_imgproc opencv_highgui)
