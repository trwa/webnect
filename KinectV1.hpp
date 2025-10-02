#pragma once

#include <array>
#include <cstdint>
#include <tuple>

#include "Frame.hpp"

namespace trwa {
    class KinectV1 {
    public:
        KinectV1(KinectV1 const &) = delete;

        void operator=(KinectV1 const &) = delete;

        ~KinectV1();

        static void run();

        static void getFrame(Frame &destination);

    private:
        static KinectV1 &getInstance();

        KinectV1();
    };
}
