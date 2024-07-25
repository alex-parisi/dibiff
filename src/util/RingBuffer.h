/// RingBuffer.h

#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>
#include <optional>

/**
 * @brief Ring Buffer
 * @details A simple thread-safe ring buffer implementation for audio data
 */
class RingBuffer {
public:
    size_t maxCapacity;
    /**
     * @brief Construct a new Ring Buffer object
     * @param capacity The capacity of the ring buffer in samples
     */
    explicit RingBuffer(size_t capacity);

    /**
     * @brief Write data to the ring buffer
     * @param data The data to write
     * @param samples The number of samples to write
     */
    void write(const float* data, size_t samples);

    /**
     * @brief Read data from the ring buffer
     * @param data The buffer to read into
     * @param samples The number of samples to read
     * @return The actual number of samples read
     */
    size_t read(float* data, size_t samples);

    /**
     * @brief Get the number of samples available in the buffer
     * @return The number of samples available
     */
    size_t available() const;

    /**
     * @brief Clear the buffer
     * @details Reset the head and tail pointers to zero
     */
    void clear();

private:
    std::vector<float> buffer;
    size_t head;
    size_t tail;
    mutable std::mutex mtx;
    std::condition_variable cv;
    size_t currentSize;
};
