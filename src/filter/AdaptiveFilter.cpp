/// AdaptiveFilter.cpp

#include "AdaptiveFilter.h"
#include "../inc/Eigen/Dense"
#include <numeric>

/**
 * @brief Constructor
 * @details Initializes the adaptive filter with the specified length
 * and step size
 * @param filterLength The length of the filter
 * @param stepSize The step size of the filter
 */
dibiff::filter::AdaptiveFilter::AdaptiveFilter(int& filterLength, float& stepSize)
: filterLength(filterLength), stepSize(stepSize), filterCoefficients(filterLength, 0.0f), buffer(filterLength, 0.0f) {
    name = "AdaptiveFilter";
};
/**
 * @brief Initialize
 * @details Initializes the adaptive filter connection points
 */
void dibiff::filter::AdaptiveFilter::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "AdaptiveFilterInput"));
    _inputs.push_back(input);
    reference = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "AdaptiveFilterReference"));
    _inputs.push_back(reference);
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "AdaptiveFilterOutput"));
    _outputs.push_back(output);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 * @param reference The reference sample
 */
float dibiff::filter::AdaptiveFilter::process(float sample, float reference) {
    // Shift the buffer
    std::copy(buffer.begin() + 1, buffer.end(), buffer.begin());
    buffer.back() = reference;
    // Compute the filter output
    float output = 0.0f;
    for (int i = 0; i < filterLength; ++i) {
        output += filterCoefficients[i] * buffer[i];
    }
    // Compute the error signal
    float error = sample - output;
    // Normalize the buffer to prevent large updates due to high signal power
    float bufferNorm = std::inner_product(buffer.begin(), buffer.end(), buffer.begin(), 0.0f);
    if (bufferNorm > 0.0f) {
        bufferNorm = std::sqrt(bufferNorm);
    } else {
        bufferNorm = 1.0f;  // Prevent division by zero
    }
    // Update the filter coefficients using the LMS algorithm with gradient clipping
    float maxUpdate = 0.1f;  // Maximum update to prevent large coefficient changes
    for (int i = 0; i < filterLength; ++i) {
        float update = stepSize * error * buffer[i] / bufferNorm;
        update = std::min(std::max(update, -maxUpdate), maxUpdate);  // Clip the update
        filterCoefficients[i] += update;
        // Check for NaN and handle it
        if (std::isnan(filterCoefficients[i])) {
            filterCoefficients[i] = 0.0f;  // Reset the coefficient to zero if NaN is detected
        }
    }
    return error;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::filter::AdaptiveFilter::process() {
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> inData = *input->getData();
        int inBlockSize = input->getBlockSize();
        std::vector<float> out(inBlockSize, 0.0f);
        output->setData(out, inBlockSize);
    } else if (!reference->isConnected()) {
        /// If no reference is connected, just pass the input through
        std::vector<float> inData = *input->getData();
        int inBlockSize = input->getBlockSize();
        output->setData(inData, inBlockSize);
    } else if (input->isReady() && reference->isReady()) {
        std::vector<float> inData = *input->getData();
        std::vector<float> refData = *reference->getData();
        int inBlockSize = input->getBlockSize();
        int refBlockSize = reference->getBlockSize();
        if (inBlockSize != refBlockSize) {
            throw std::runtime_error("Block sizes do not match");
        }
        Eigen::VectorXf x(inBlockSize), r(inBlockSize), y(inBlockSize);
        for (int i = 0; i < inBlockSize; ++i) {
            x(i) = inData[i];
            r(i) = refData[i];
        }
        for (int i = 0; i < inBlockSize; ++i) {
            y(i) = process(x(i), r(i));
        }
        std::vector<float> out(inBlockSize);
        for (int i = 0; i < inBlockSize; ++i) {
            out[i] = y(i);
        }
        output->setData(out, inBlockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the filter
 * @details Resets the filter coefficients and buffer
 */
void dibiff::filter::AdaptiveFilter::reset() {
    filterCoefficients = std::vector<float>(filterLength, 0.0f);
    buffer = std::vector<float>(filterLength, 0.0f);
}
/**
 * @brief Clear the filter
 * @details Clears the buffer
 */
void dibiff::filter::AdaptiveFilter::clear() {
    buffer = std::vector<float>(filterLength, 0.0f);
}
/**
 * @brief Check if the filter is finished processing
 * @return True if the filter is finished processing, false otherwise
 */
bool dibiff::filter::AdaptiveFilter::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && reference->isConnected() && reference->isReady() && reference->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::filter::AdaptiveFilter::isReadyToProcess() const {
        if (!reference->isConnected()) {
        return input->isConnected() && input->isReady() && !processed;
    } else if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && reference->isReady() && !processed;
}
/**
 * Create a new adaptive filter object
 * @param filterLength The length of the filter
 * @param stepSize The step size of the filter
 */
std::shared_ptr<dibiff::filter::AdaptiveFilter> dibiff::filter::AdaptiveFilter::create(int& filterLength, float& stepSize) {
    auto instance = std::make_shared<dibiff::filter::AdaptiveFilter>(filterLength, stepSize);
    instance->initialize();
    return instance;
}