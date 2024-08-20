/// RingBuffer.h

#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>
#include <optional>

/**
 * @brief Ring Buffer
 * @details A simple thread-safe ring buffer implementation for data
 */
template<typename T>
class RingBuffer {
public:
    std::size_t maxCapacity;
    /**
     * @brief Construct a new Ring Buffer object
     * @param capacity The capacity of the ring buffer in samples
     */
    explicit RingBuffer(std::size_t capacity);

    /**
     * @brief Write data to the ring buffer
     * @param data The data to write
     * @param samples The number of samples to write
     */
    void write(const std::vector<T> data, std::size_t samples);

    /**
     * @brief Read data from the ring buffer
     * @param data The buffer to read into
     * @param samples The number of samples to read
     * @return The actual number of samples read
     */
    std::vector<T> read(std::size_t samples);

    /**
     * @brief Get the number of samples available in the buffer
     * @return The number of samples available
     */
    std::size_t available() const;

    /**
     * @brief Clear the buffer
     * @details Reset the head and tail pointers to zero
     */
    void clear();

private:
    std::vector<T> buffer;
    std::size_t head;
    std::size_t tail;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::size_t currentSize;
};

/**
 * @brief Construct a new Ring Buffer object
 * @param capacity The capacity of the ring buffer in samples
 */
template<typename T>
RingBuffer<T>::RingBuffer(std::size_t capacity)
    : buffer(capacity), head(0), tail(0), maxCapacity(capacity), currentSize(0) {}
/**
 * @brief Write data to the ring buffer
 * @param data The data to write
 * @param samples The number of samples to write
 */
template<typename T>
void RingBuffer<T>::write(const std::vector<T> data, std::size_t samples) {
    std::unique_lock<std::mutex> lock(mtx);
    for (std::size_t i = 0; i < samples; ++i) {
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
 * @param samples The number of samples to read
 * @return The actual number of samples read
 */
template<typename T>
std::vector<T> RingBuffer<T>::read(std::size_t samples) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]{ return currentSize > 0; });
    std::size_t readCount = 0;
    std::vector<T> data(samples);
    for (; readCount < samples && currentSize > 0; ++readCount) {
        data[readCount] = (buffer[head]);
        head = (head + 1) % maxCapacity;
        --currentSize;
    }
    return data;
}
/**
 * @brief Get the number of samples available in the buffer
 * @return The number of samples available
 */
template<typename T>
std::size_t RingBuffer<T>::available() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentSize;
}
/**
 * @brief Clear the buffer
 * @details Reset the head and tail pointers to zero
 */
template<typename T>
void RingBuffer<T>::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    head = 0;
    tail = 0;
    currentSize = 0;
}