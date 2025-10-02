#pragma once

#include <cstdint>
#include <array>
#include <tuple>

namespace trwa {
    class Frame {
    public:
        struct RGB {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };

        struct XYZ {
            double x;
            double y;
            uint16_t z;
        };

        static constexpr auto ROWS = 480;

        static constexpr auto COLS = 640;

        void setVideo(uint8_t const video[ROWS * COLS * 3], uint32_t ts);

        void setDepth(uint16_t const depth[ROWS * COLS], uint32_t ts);

        void getVideo(uint8_t video[ROWS * COLS * 3]) const;

        void getDepth(uint16_t depth[ROWS * COLS]) const;

    private:
        std::array<std::array<std::tuple<RGB, XYZ>, COLS>, ROWS> data;
        uint32_t tsVideo{};
        uint32_t tsDepth{};
    };
}
