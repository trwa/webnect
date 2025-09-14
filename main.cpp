#include <iostream>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "KinectV1.hpp"


static void showDepth() {
    static float data_f64[480 * 640] = {0};
    static std::vector<uint16_t> data_u16;
    trwa::depthBuffer.load(data_u16);
    for (size_t i = 0; i < 480 * 640; ++i) {
        data_f64[i] = data_u16.at(i) / 2048.0f;
    }
    auto const frame = cv::Mat(480, 640, CV_32FC1, data_f64);
    cv::imshow("depth", frame);
}

static void showVideo() {
    static std::vector<uint8_t> data{};
    trwa::videoBuffer.load(data);
    auto const frame = cv::Mat(480, 640, CV_8UC3, data.data());
    cv::imshow("video", frame);
}

int main() {
    auto &kinect = trwa::KinectV1::getInstance();

    cv::namedWindow("depth", cv::WINDOW_NORMAL);
    cv::resizeWindow("depth", 640, 480);

    cv::namedWindow("video", cv::WINDOW_NORMAL);
    cv::resizeWindow("video", 640, 480);

    std::thread kinectThread(
        [&kinect]() {
            kinect.runForever();
        }
    );

    while (true) {
        showDepth();
        showVideo();
        cv::waitKey(1);
    }
    kinectThread.join();
    return 0;
}
