/// Gain.cpp

#include "Gain.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the gain object with a certain value
 * @param value The gain of the object in dB
 */
dibiff::level::Gain::Gain(float& value)
: dibiff::graph::AudioObject(), _valuedB(value), _value(0.0f) {
    name = "Gain";
};
/**
 * @brief Initialize
 * @details Initializes the gain connection points
 */
void dibiff::level::Gain::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "GainInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "GainOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::level::Gain::process(float sample) {
    return sample * _value;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::level::Gain::process() {
    /// Update value
    _value = std::pow(10.0f, _valuedB / 20.0f);
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> out(input->getBlockSize(), 0.0f);
        output->setData(out, input->getBlockSize());
        markProcessed();
    } else if (input->isReady()) {
        const std::vector<float>& audioData = input->getData();
        const int blockSize = input->getBlockSize();
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = audioData[i];
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
 * @brief Check if the gain is finished processing
 * @return True if the gain is finished processing, false otherwise
 */
bool dibiff::level::Gain::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::level::Gain::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new gain object
 * @param value The gain of the object in dB
 */
std::unique_ptr<dibiff::level::Gain> dibiff::level::Gain::create(float& value) {
    auto instance = std::make_unique<dibiff::level::Gain>(value);
    instance->initialize();
    return std::move(instance);
}