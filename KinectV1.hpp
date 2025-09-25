#pragma once

#include <array>
#include <cstdint>
#include <tuple>

namespace trwa {
    class KinectV1 {
    public:
        class RGBDFrame {
        public:
            static constexpr auto ROWS = 480;

            static constexpr auto COLS = 640;

            void setVideo(uint8_t const video[ROWS * COLS * 3], uint32_t ts);

            void setDepth(uint16_t const depth[ROWS * COLS], uint32_t ts);

            void getVideo(uint8_t video[ROWS * COLS * 3]) const;

            void getDepth(uint16_t depth[ROWS * COLS]) const;

        private:
            std::array<std::array<std::tuple<uint8_t, uint8_t, uint8_t, uint16_t>, COLS>, ROWS> data;
            uint32_t tsVideo{};
            uint32_t tsDepth{};
        };

        KinectV1(KinectV1 const &) = delete;

        void operator=(KinectV1 const &) = delete;

        ~KinectV1();

        static void run();

        static void getFrame(RGBDFrame &destination);

    private:
        static KinectV1 &getInstance();

        KinectV1();
    };
}
