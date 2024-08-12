/// DigitalBiquadFilter.cpp

#include "DigitalBiquadFilter.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the filter with custom coefficients
 * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
 * b0, b1 b2, a0, a1, a2
 * @see reset
 * @see dibiff::Coefficients
 */
dibiff::filter::DigitalBiquadFilter::DigitalBiquadFilter(dibiff::filter::Coefficients& coeffs) 
: dibiff::graph::AudioObject(), _coeffs(coeffs) {
    name = "DigitalBiquadFilter";
    reset();
};
/**
 * @brief Initialize
 * @details Initializes the filter state variables and connection points
 */
void dibiff::filter::DigitalBiquadFilter::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "DigitalBiquadFilterInput");
    _inputs.push_back(input);
    output = std::make_shared<dibiff::graph::AudioOutput>(shared_from_this(), "DigitalBiquadFilterOutput");
    _outputs.push_back(output);
    /// Check for divide by zero
    if (_coeffs.a0 == 0) {
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
    float output = ((_coeffs.b0 / _coeffs.a0) * sample) + ((_coeffs.b1 / _coeffs.a0) * x1) + ((_coeffs.b2 / _coeffs.a0) * x2) - ((_coeffs.a1 / _coeffs.a0) * y1) - ((_coeffs.a2 / _coeffs.a0) * y2);
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
void dibiff::filter::DigitalBiquadFilter::setCoefficients(dibiff::filter::Coefficients& coeffs) {
    _coeffs = coeffs;
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
std::shared_ptr<dibiff::filter::DigitalBiquadFilter> dibiff::filter::DigitalBiquadFilter::create(dibiff::filter::Coefficients& coeffs) {
    auto instance = std::make_shared<DigitalBiquadFilter>(coeffs);
    instance->initialize();
    return instance;
} 