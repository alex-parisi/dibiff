/// RingBuffer.cpp

#include "RingBuffer.h"

#include <iostream>

/**
 * @brief Construct a new Ring Buffer object
 * @param capacity The capacity of the ring buffer in samples
 */
RingBuffer::RingBuffer(size_t capacity)
    : buffer(capacity), head(0), tail(0), maxCapacity(capacity), currentSize(0) {}
/**
 * @brief Write data to the ring buffer
 * @param data The data to write
 * @param samples The number of samples to write
 */
void RingBuffer::write(const float* data, size_t samples) {
    std::unique_lock<std::mutex> lock(mtx);
    for (size_t i = 0; i < samples; ++i) {
        if (currentSize < maxCapacity) {
            buffer[tail] = data[i];
            tail = (tail + 1) % maxCapacity;
            ++currentSize;
        } else {
            // Buffer overrun
            break;
        }
    }
    cv.notify_one();
}
/**
 * @brief Read data from the ring buffer
 * @param data The buffer to read into
 * @param samples The number of samples to read
 * @return The actual number of samples read
 */
size_t RingBuffer::read(float* data, size_t samples) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]{ return currentSize > 0; });

    size_t readCount = 0;
    for (; readCount < samples && currentSize > 0; ++readCount) {
        data[readCount] = buffer[head];
        head = (head + 1) % maxCapacity;
        --currentSize;
    }
    return readCount;
}
/**
 * @brief Get the number of samples available in the buffer
 * @return The number of samples available
 */
size_t RingBuffer::available() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentSize;
}
/**
 * @brief Clear the buffer
 * @details Reset the head and tail pointers to zero
 */
void RingBuffer::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    head = 0;
    tail = 0;
    currentSize = 0;
}