#pragma once

#include <format>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace trwa {
    template<typename T>
    class AtomicBuffer {
    public:
        explicit AtomicBuffer(size_t size) {
            buffer_.resize(size);
        };

        AtomicBuffer() = delete;

        void store(T const *data, size_t size) {
            mutex_.lock();
            if (size != buffer_.size()) {
                mutex_.unlock();
                throw std::runtime_error(std::format("invalid video size {}, expected {}", size, buffer_.size()));
            }
            for (size_t i = 0; i < size; ++i) {
                buffer_[i] = data[i];
            }
            mutex_.unlock();
        }

        void load(std::vector<T> &out) {
            mutex_.lock();
            out = buffer_;
            mutex_.unlock();
        }

    private:
        std::mutex mutex_{};
        std::vector<T> buffer_{};
    };
}
