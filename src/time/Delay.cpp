/// Delay.cpp

#include "Delay.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the delay object with a certain delay time
 * @param delayTime The delay time of the object in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::time::Delay::Delay(float delayTime, float sampleRate)
: dibiff::graph::AudioObject(), delayTime(delayTime), sampleRate(sampleRate) {
    name = "Delay";
};
/**
 * @brief Initialize
 * @details Initializes the delay connection points and buffer
 */
void dibiff::time::Delay::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "DelayInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "DelayOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
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
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> out(input->getBlockSize(), 0.0f);
        output->setData(out, input->getBlockSize());
        markProcessed();
    } else if (input->isReady()) {
        const std::vector<float>& data = input->getData();
        const int blockSize = input->getBlockSize();
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
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new delay object
 * @param delayTime The delay time of the object in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::time::Delay> dibiff::time::Delay::create(float delayTime, float sampleRate) {
    auto instance = std::make_unique<dibiff::time::Delay>(delayTime, sampleRate);
    instance->initialize();
    return std::move(instance);
}