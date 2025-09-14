#pragma once

#include "libfreenect.h"

namespace trwa {
    class kinectv1 {
    public:
        kinectv1();

    private:
        freenect_context *ctx = nullptr;
        freenect_device *dev = nullptr;
    };
}

