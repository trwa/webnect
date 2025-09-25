#pragma once

#include <array>
#include <format>
#include <functional>
#include <mutex>

namespace trwa {
    template<typename T>
    class Atomic {
    public:
        void operator()(std::function<void(T const &)> f) {
            mutex_.lock();
            f(data_);
            mutex_.unlock();
        }

        void operator()(std::function<void(T &)> f) {
            mutex_.lock();
            f(data_);
            mutex_.unlock();
        }

    private:
        std::mutex mutex_{};
        T data_{};
    };
}
