/// ExpanderGate.cpp

#include "ExpanderGate.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::gate::ExpanderGate::getName() const { return "ExpanderGate"; }
/**
 * @brief Constructor
 * @details Initializes the expander gate object with a certain threshold,
 * ratio, attack time, release time, and sample rate
 * @param threshold The threshold level in dB
 * @param ratio The expansion ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::gate::ExpanderGate::ExpanderGate(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate)
: dibiff::graph::AudioObject(), 
    threshold(threshold), ratio(ratio), attackTime(attackTime), releaseTime(releaseTime), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the expander gate connection points and envelope
 */
void dibiff::gate::ExpanderGate::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "ExpanderGateInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "ExpanderGateOutput")); 
    attackCoefficient = exp(-1.0 / (attackTime * sampleRate / 1000.0f));
    releaseCoefficient = exp(-1.0 / (releaseTime * sampleRate / 1000.0f));
    thresholdLevel = pow(10.0f, threshold / 20.0f); // Convert dB to linear
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 * @return The output sample
 */
float dibiff::gate::ExpanderGate::process(float sample) {
    float inputLevel = fabs(sample);
    if (inputLevel > thresholdLevel) {
        envelope = attackCoefficient * (envelope - inputLevel) + inputLevel;
    } else {
        envelope = releaseCoefficient * envelope;
    }
    if (inputLevel < thresholdLevel) {
        float gainReduction = pow(envelope / thresholdLevel, ratio - 1.0f);
        return sample * gainReduction;
    }
    return sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param buffer The input buffer
 * @param blockSize The size of the block
 */
void dibiff::gate::ExpanderGate::process() {
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
 * @brief Reset the expander gate
 * @details Resets the envelope to zero
 */
void dibiff::gate::ExpanderGate::reset() {
    envelope = 0.0f;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::gate::ExpanderGate::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::gate::ExpanderGate::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::gate::ExpanderGate::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the expander gate is finished processing
 * @return True if the expander gate is finished processing, false otherwise
 */
bool dibiff::gate::ExpanderGate::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::gate::ExpanderGate::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new expander gate object
 * @param threshold The threshold level in dB
 * @param ratio The expansion ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::gate::ExpanderGate> dibiff::gate::ExpanderGate::create(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate) {
    auto instance = std::make_shared<dibiff::gate::ExpanderGate>(threshold, ratio, attackTime, releaseTime, sampleRate);
    instance->initialize();
    return instance;
}