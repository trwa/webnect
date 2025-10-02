cmake_minimum_required(VERSION 3.31)
project("kinect")

set(CMAKE_CXX_STANDARD 23)

add_executable("kinect")
target_sources("kinect" PUBLIC "main.cpp" "KinectV1.cpp" "mongoose.c")

add_executable("jeje")
target_sources("jeje" PUBLIC "jeje.c" "mongoose.c")


target_include_directories("kinect" PRIVATE "/usr/include/libfreenect" "/usr/include/opencv4")
target_link_libraries("kinect" PRIVATE "atomic" "freenect" "opencv_core" "opencv_highgui" "opencv_imgproc")
