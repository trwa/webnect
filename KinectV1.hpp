#pragma once

#include <libfreenect.h>

#include "AtomicBuffer.hpp"

namespace trwa {
    extern AtomicBuffer<uint16_t> depthBuffer;
    extern AtomicBuffer<uint8_t> videoBuffer;

    class KinectV1 {
    public:
        static KinectV1 &getInstance();

        KinectV1(KinectV1 const &) = delete;

        void operator=(KinectV1 const &) = delete;

        ~KinectV1();

        void runForever();

    private:
        KinectV1();
    };
}
