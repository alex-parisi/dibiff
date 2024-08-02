/// DigitalBiquadFilter.cpp

#include "DigitalBiquadFilter.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the filter with default values
 * @see reset
 */
dibiff::filter::DigitalBiquadFilter::DigitalBiquadFilter() 
: dibiff::graph::AudioObject(), 
  b0(0), b1(0), b2(0), a0(0), a1(0), a2(0) {
    name = "DigitalBiquadFilter";
};
/**
 * @brief Constructor
 * @details Initializes the filter with custom coefficients
 * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
 * b0, b1 b2, a0, a1, a2
 * @see reset
 * @see dibiff::Coefficients
 */
dibiff::filter::DigitalBiquadFilter::DigitalBiquadFilter(const dibiff::filter::Coefficients& coeffs) 
: dibiff::graph::AudioObject(), 
    b0(coeffs.b0), b1(coeffs.b1), b2(coeffs.b2), 
    a0(coeffs.a0), a1(coeffs.a1), a2(coeffs.a2) {
    name = "DigitalBiquadFilter";
    reset();
};
/**
 * @brief Initialize
 * @details Initializes the filter state variables and connection points
 */
void dibiff::filter::DigitalBiquadFilter::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "DigitalBiquadFilterInput");
    output = std::make_shared<dibiff::graph::AudioOutput>(shared_from_this(), "DigitalBiquadFilterOutput");
    /// Check for divide by zero
    if (a0 == 0) {
        throw std::invalid_argument("a0 cannot be zero");
    }
    /// Initialize state variables
    reset();
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::filter::DigitalBiquadFilter::process(float sample) {
    float output = ((b0 / a0) * sample) + ((b1 / a0) * x1) + ((b2 / a0) * x2) - ((a1 / a0) * y1) - ((a2 / a0) * y2);
    x2 = x1;
    x1 = sample;
    y2 = y1;
    y1 = output;
    iter++;
    return output;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of samples of audio data
 */
void dibiff::filter::DigitalBiquadFilter::process() {
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> out(input->getBlockSize(), 0.0f);
        output->setData(out, input->getBlockSize());
        markProcessed();
    } else if (input->isReady()) {
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
 * @brief Set the filter coefficients
 * @details Sets the filter coefficients to custom values
 * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
 * b0, b1 b2, a0, a1, a2
 * @see dibiff::Coefficients 
 */
void dibiff::filter::DigitalBiquadFilter::setCoefficients(const dibiff::filter::Coefficients& coeffs) {
    b0 = coeffs.b0;
    b1 = coeffs.b1;
    b2 = coeffs.b2;
    a0 = coeffs.a0;
    a1 = coeffs.a1;
    a2 = coeffs.a2;
    reset();
}
/**
 * @brief Reset the filter
 * @details Resets the filter state variables
 */
void dibiff::filter::DigitalBiquadFilter::reset() {
    x1 = 0;
    x2 = 0;
    y1 = 0;
    y2 = 0;
    iter = 0;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::filter::DigitalBiquadFilter::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::filter::DigitalBiquadFilter::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::filter::DigitalBiquadFilter::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the filter is finished processing
 * @return True if the filter is finished processing, false otherwise
 */
bool dibiff::filter::DigitalBiquadFilter::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::filter::DigitalBiquadFilter::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * @brief Create a new filter object
 * @details Creates a new filter object with custom coefficients
 * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
 * b0, b1 b2, a0, a1, a2
 */
std::shared_ptr<dibiff::filter::DigitalBiquadFilter> dibiff::filter::DigitalBiquadFilter::create(dibiff::filter::Coefficients coeffs) {
    auto instance = std::make_shared<DigitalBiquadFilter>(coeffs);
    instance->initialize();
    return instance;
} 