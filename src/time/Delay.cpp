/// Delay.cpp

#include "Delay.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::time::Delay::getName() const { return "Delay"; }
/**
 * @brief Constructor
 * @details Initializes the delay object with a certain delay time
 * @param delayTime The delay time of the object in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::time::Delay::Delay(float delayTime, float sampleRate)
: dibiff::graph::AudioObject(),
    delayTime(delayTime), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the delay connection points and buffer
 */
void dibiff::time::Delay::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "DelayInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "DelayOutput"));
    buffer.resize(static_cast<int>(delayTime * sampleRate / 1000.0f));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::time::Delay::process(float sample) {
    float output = buffer[bufferIndex];
    buffer[bufferIndex] = sample;
    bufferIndex = (bufferIndex + 1) % buffer.size();
    return output;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::time::Delay::process() {
    if (input->isReady()) {
        std::vector<float> data = *input->getData();
        int blockSize = input->getBlockSize();
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = data[i];
        }
        for (int i = 0; i < blockSize; ++i) {
            y(i) = process(x(i));
        }
        std::vector<float> out(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            out[i] = y(i);
        }
        output->setData(out, blockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the delay
 * @details Resets the delay buffer
 */
void dibiff::time::Delay::reset() {
    buffer = std::vector<float>(buffer.size(), 0.0f);
}
/**
 * @brief Clear the delay
 * @details Clears the delay buffer
 */
void dibiff::time::Delay::clear() {
    reset();
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::time::Delay::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::time::Delay::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::time::Delay::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the delay is finished processing
 * @return True if the delay is finished processing, false otherwise
 */
bool dibiff::time::Delay::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::time::Delay::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new delay object
 * @param delayTime The delay time of the object in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::time::Delay> dibiff::time::Delay::create(float delayTime, float sampleRate) {
    auto instance = std::make_shared<dibiff::time::Delay>(delayTime, sampleRate);
    instance->initialize();
    return instance;
}