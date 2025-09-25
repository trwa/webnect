#include "KinectV1.hpp"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <format>
#include <thread>

#include <libfreenect/libfreenect.h>

#include "Atomic.hpp"


namespace trwa {
    static freenect_context *ctx_{};

    static freenect_device *dev_{};

    static Atomic<KinectV1::RGBDFrame> frameBuffer_;

    static void atomicStoreDepth_(freenect_device *_, void *data, uint32_t ts) {
        auto const store = [data, ts](KinectV1::RGBDFrame &frame) {
            frame.setDepth(static_cast<uint16_t const *>(data), ts);
        };
        frameBuffer_(store);
    }

    static void atomicStoreVideo_(freenect_device *_, void *data, uint32_t ts) {
        auto const store = [data, ts](KinectV1::RGBDFrame &frame) {
            frame.setVideo(static_cast<uint8_t const *>(data), ts);
        };
        frameBuffer_(store);
    }


    void KinectV1::RGBDFrame::setVideo(uint8_t const video[ROWS * COLS * 3], uint32_t const ts) {
        for (size_t i = 0; i < ROWS; ++i) {
            for (size_t j = 0; j < COLS; ++j) {
                auto const pixel = video + (COLS * i + j) * 3;
                std::get<0>(data[i][j]) = *(pixel + 0);
                std::get<1>(data[i][j]) = *(pixel + 1);
                std::get<2>(data[i][j]) = *(pixel + 2);
                tsVideo = ts;
            }
        }
    }

    void KinectV1::RGBDFrame::setDepth(uint16_t const depth[ROWS * COLS], uint32_t const ts) {
        for (size_t i = 0; i < ROWS; ++i) {
            for (size_t j = 0; j < COLS; ++j) {
                std::get<3>(data[i][j]) = depth[COLS * i + j];
                tsDepth = ts;
            }
        }
    }

    void KinectV1::RGBDFrame::getVideo(uint8_t video[ROWS * COLS * 3]) const {
        for (size_t i = 0; i < ROWS; ++i) {
            for (size_t j = 0; j < COLS; ++j) {
                auto const pixel = video + (COLS * i + j) * 3;
                *(pixel + 0) = std::get<0>(data[i][j]);
                *(pixel + 1) = std::get<1>(data[i][j]);
                *(pixel + 2) = std::get<2>(data[i][j]);
            }
        }
    }

    void KinectV1::RGBDFrame::getDepth(uint16_t depth[ROWS * COLS]) const {
        for (size_t i = 0; i < ROWS; ++i) {
            for (size_t j = 0; j < COLS; ++j) {
                depth[COLS * i + j] = std::get<3>(data[i][j]);
            }
        }
    }

    KinectV1 &KinectV1::getInstance() {
        static KinectV1 instance{};
        return instance;
    }

    KinectV1::~KinectV1() {
        if (dev_ != nullptr) {
            freenect_close_device(dev_);
            dev_ = nullptr;
        }
        if (ctx_ != nullptr) {
            freenect_shutdown(ctx_);
        }
    }

    void KinectV1::run() {
        auto const &_ = getInstance();

        int32_t ret = 0;

        freenect_set_video_callback(dev_, atomicStoreVideo_);
        freenect_set_depth_callback(dev_, atomicStoreDepth_);

        ret = freenect_start_video(dev_);
        if (ret != 0) {
            throw std::runtime_error(std::format("failed to start video acquisition with error {}", ret));
        }

        ret = freenect_start_depth(dev_);
        if (ret != 0) {
            throw std::runtime_error(std::format("failed to start depth acquisition with error {}", ret));
        }

        while (true) {
            freenect_process_events(ctx_);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    KinectV1::KinectV1() {
        int32_t ret = 0;

        ret = freenect_init(&ctx_, nullptr);
        if (ret != 0) {
            throw std::runtime_error(std::format("could not initialize freenect with error {}", ret));
        }
        if (ctx_ == nullptr) {
            throw std::runtime_error("could not get context");
        }

        ret = freenect_open_device(ctx_, &dev_, 0);
        if (ret != 0) {
            throw std::runtime_error(std::format("could not open device with error {}", ret));
        }
        if (dev_ == nullptr) {
            throw std::runtime_error("could not get device");
        }

        auto frame_mode = freenect_get_current_depth_mode(dev_);
        frame_mode.depth_format = FREENECT_DEPTH_REGISTERED;

        ret = freenect_set_depth_mode(dev_, frame_mode);
        if (ret != 0) {
            throw std::runtime_error(std::format("could not set depth mode with error {}", ret));
        }
    }

    void KinectV1::getFrame(RGBDFrame &destination) {
        std::function<void(RGBDFrame const &)> const load = [&destination](RGBDFrame const &source) {
            destination = source;
        };
        frameBuffer_(load);
    }
};
