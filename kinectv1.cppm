#include "kinectv1.hpp"

#include <iostream>


namespace trwa {
    kinectv1::kinectv1() {


        ret = freenect_init(&ctx, nullptr);
        if (ret != 0) {
            std::cerr << "error: could not initialize context with error " << ret << '\n';
            return 1;
        }
        if (ctx == nullptr) {
            std::cerr << "error: could not get context\n";
            return 1;
        }

        ret = freenect_open_device(ctx, &dev, 0);
        if (ret != 0) {
            std::cerr << "error: could not open device with error " << ret << '\n';
            return 1;
        }
        if (dev == nullptr) {
            std::cerr << "error: could not get device\n";
            return 1;
        }

        auto frame_mode = freenect_get_current_depth_mode(dev);
        frame_mode.depth_format = FREENECT_DEPTH_REGISTERED;

        ret = freenect_set_depth_mode(dev, frame_mode);
        if (ret != 0) {
            std::cerr << "error: could not set depth mode with error " << ret << '\n';
            return 1;
        }
    }
}
