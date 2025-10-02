#include <array>
#include <iostream>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "KinectV1.hpp"


int main() {
    cv::namedWindow("video", cv::WINDOW_NORMAL);
    cv::namedWindow("depth", cv::WINDOW_NORMAL);

    std::thread kinectThread(
        []() {
            trwa::KinectV1::run();
        }
    );

    while (true) {
        static trwa::Frame frame;
        trwa::KinectV1::getFrame(frame);

        static uint8_t video[trwa::Frame::ROWS * trwa::Frame::COLS * 3];
        frame.getVideo(video);

        static uint16_t depth[trwa::Frame::ROWS * trwa::Frame::COLS];
        frame.getDepth(depth);

        static double fDepth[trwa::Frame::ROWS * trwa::Frame::COLS];
        for (size_t i = 0; i < trwa::Frame::ROWS * trwa::Frame::COLS; ++i) {
            fDepth[i] = depth[i] / 2048.0;
        }

        cv::imshow(
            "video",
            cv::Mat(trwa::Frame::ROWS, trwa::Frame::COLS, CV_8UC3, video)
        );
        cv::imshow(
            "depth",
            cv::Mat(trwa::Frame::ROWS, trwa::Frame::COLS, CV_64FC1, fDepth)
        );

        cv::waitKey(1);
    }
    kinectThread.join();
    return 0;
}
