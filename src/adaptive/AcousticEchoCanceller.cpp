/// AcousticEchoCanceller.cpp

#include "AcousticEchoCanceller.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the acoustic echo canceller with a certain filter length,
 * step size, buffer size, and sample rate
 * @param filterLength The length of the adaptive filter
 * @param stepSize The step size of the adaptive filter
 * @param sampleRate The sample rate of the input signal
 */
dibiff::adaptive::AcousticEchoCanceller::AcousticEchoCanceller(int& filterLength, float& stepSize)
: dibiff::graph::AudioObject(), 
  filterLength(filterLength), stepSize(stepSize) {
    name = "AcousticEchoCanceller";
};
/**
 * @brief Initialize
 * @details Initializes the acoustic echo canceller connection points and adaptive filter
 */
void dibiff::adaptive::AcousticEchoCanceller::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "AcouticEchoCancellerInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto r = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "AcousticEchoCancellerReference"));
    _inputs.emplace_back(std::move(r));
    reference = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "AcousticEchoCancellerOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());

    adaptiveFilter = std::make_unique<dibiff::filter::AdaptiveFilter>(filterLength, stepSize);
    adaptiveFilter->initialize();
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param inputBuffer The input buffer
 * @param referenceBuffer The reference buffer
 * @param blockSize The size of the block
 */
void dibiff::adaptive::AcousticEchoCanceller::process() {
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        const std::vector<float>& inData = input->getData();
        const int inBlockSize = input->getBlockSize();
        std::vector<float> out(inBlockSize, 0.0f);
        output->setData(out, inBlockSize);
    } else if (!reference->isConnected()) {
        /// If no reference is connected, just pass the input through
        const std::vector<float>& inData = input->getData();
        const int inBlockSize = input->getBlockSize();
        output->setData(inData, inBlockSize);
    } else if (input->isReady() && reference->isReady()) {
        /// If both are connected and ready, process the data
        const std::vector<float>& inData = input->getData();
        const std::vector<float>& refData = reference->getData();
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
            y(i) = adaptiveFilter->process(x(i), r(i));
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
 * @brief Reset the acoustic echo canceller
 * @details Resets the adaptive filter
 */
void dibiff::adaptive::AcousticEchoCanceller::reset() {
    adaptiveFilter->reset();
}
/**
 * @brief Clear the acoustic echo canceller
 * @details Clears the adaptive filter
 */
void dibiff::adaptive::AcousticEchoCanceller::clear() {
    adaptiveFilter->clear();
}
/**
 * @brief Check if the AEC is finished processing
 * @return True if the AEC is finished processing, false otherwise
 */
bool dibiff::adaptive::AcousticEchoCanceller::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && reference->isConnected() && reference->isReady() && reference->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::adaptive::AcousticEchoCanceller::isReadyToProcess() const {
    if (!reference->isConnected()) {
        return input->isConnected() && input->isReady() && !processed;
    } else if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && reference->isReady() && !processed;
}
/**
 * Create a new acoustic echo canceller object
 * @param filterLength The length of the adaptive filter
 * @param stepSize The step size of the adaptive filter
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::adaptive::AcousticEchoCanceller> dibiff::adaptive::AcousticEchoCanceller::create(int& filterLength, float& stepSize) {
    auto instance = std::make_unique<dibiff::adaptive::AcousticEchoCanceller>(filterLength, stepSize);
    instance->initialize();
    return std::move(instance);
}