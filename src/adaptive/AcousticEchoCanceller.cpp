/// AcousticEchoCanceller.cpp

#include "AcousticEchoCanceller.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::adaptive::AcousticEchoCanceller::getName() const { return "AcousticEchoCanceller"; }
/**
 * @brief Constructor
 * @details Initializes the acoustic echo canceller with a certain filter length,
 * step size, buffer size, and sample rate
 * @param filterLength The length of the adaptive filter
 * @param stepSize The step size of the adaptive filter
 * @param sampleRate The sample rate of the input signal
 */
dibiff::adaptive::AcousticEchoCanceller::AcousticEchoCanceller(int filterLength, float stepSize, float sampleRate)
: dibiff::graph::AudioObject(), 
    filterLength(filterLength), stepSize(stepSize), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the acoustic echo canceller connection points and adaptive filter
 */
void dibiff::adaptive::AcousticEchoCanceller::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "AcousticEchoCancellerInput"));
    reference = std::make_shared<dibiff::graph::AudioReference>(dibiff::graph::AudioReference(shared_from_this(), "AcousticEchoCancellerReference"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "AcousticEchoCancellerOutput"));
    adaptiveFilter = std::make_shared<dibiff::filter::AdaptiveFilter>(filterLength, stepSize);
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
    if (input->isReady() && reference->isReady()) {
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
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::adaptive::AcousticEchoCanceller::getInput() { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::adaptive::AcousticEchoCanceller::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::adaptive::AcousticEchoCanceller::getReference() { return reference; };
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
    // Check if input is connected
    return input->isConnected() && input->isReady() && reference->isConnected() && reference->isReady() && !processed;
}
/**
 * Create a new acoustic echo canceller object
 * @param filterLength The length of the adaptive filter
 * @param stepSize The step size of the adaptive filter
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::adaptive::AcousticEchoCanceller> dibiff::adaptive::AcousticEchoCanceller::create(int filterLength, float stepSize, float sampleRate) {
    auto instance = std::make_shared<dibiff::adaptive::AcousticEchoCanceller>(filterLength, stepSize, sampleRate);
    instance->initialize();
    return instance;
}