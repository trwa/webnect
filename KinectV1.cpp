#include "KinectV1.hpp"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <format>
#include <thread>

#include <libfreenect/libfreenect.h>

#include "AtomicBuffer.hpp"


namespace trwa {
    AtomicBuffer<uint16_t> depthBuffer(640 * 480);
    AtomicBuffer<uint8_t> videoBuffer(640 * 480 * 3);

    static freenect_context *ctx_ = nullptr;
    static freenect_device *dev_ = nullptr;

    static void onDepth_(freenect_device *dev, void *data, uint32_t timestamp) {
        depthBuffer.store(static_cast<uint16_t *>(data), 640 * 480);
    }

    static void onVideo_(freenect_device *dev, void *data, uint32_t timestamp) {
        videoBuffer.store(static_cast<uint8_t *>(data), 640 * 480 * 3);
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

    void KinectV1::runForever() {
        int32_t ret = 0;

        freenect_set_video_callback(dev_, onVideo_);
        freenect_set_depth_callback(dev_, onDepth_);

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
};
