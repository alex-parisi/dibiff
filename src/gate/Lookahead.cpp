/// LookaheadGate.cpp

#include "LookaheadGate.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::gate::LookaheadGate::getName() const { return "LookaheadGate"; }
/**
 * @brief Constructor
 * @details Initializes the lookahead gate object with a certain threshold,
 * attack time, release time, lookahead time, and sample rate
 * @param threshold The threshold level in dB
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param lookaheadTime The lookahead time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::gate::LookaheadGate::LookaheadGate(float threshold, float attackTime, float releaseTime, float lookaheadTime, float sampleRate)
: dibiff::graph::AudioObject(), 
    threshold(threshold), attackTime(attackTime), releaseTime(releaseTime), lookaheadTime(lookaheadTime), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the lookahead gate connection points and envelope
 */
void dibiff::gate::LookaheadGate::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "LookaheadGateInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "LookaheadGateOutput"));
    attackCoefficient = std::expf(-1.0f / (attackTime * sampleRate / 1000.0f));
    releaseCoefficient = std::expf(-1.0f / (releaseTime * sampleRate / 1000.0f));
    thresholdLevel = std::powf(10.0f, threshold / 20.0f);
    lookaheadSamples = static_cast<int>(lookaheadTime * sampleRate / 1000.0f);
    delayBuffer.resize(lookaheadSamples, 0.0f);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 */
float dibiff::gate::LookaheadGate::process(float sample) {
    float sidechainInput = delayBuffer[bufferIndex];
    float sidechainLevel = std::fabsf(sidechainInput);
    if (sidechainLevel > thresholdLevel) {
        envelope = attackCoefficient * (envelope - sidechainLevel) + sidechainLevel;
    } else {
        envelope = releaseCoefficient * envelope;
    }
    float out;
    if (sidechainLevel < thresholdLevel) {
        float gainReduction = envelope < thresholdLevel ? 0.0f : 1.0f;
        out = sample * gainReduction;
    } else {
        out = sample;
    }
    delayBuffer[bufferIndex] = sample;
    bufferIndex = (bufferIndex + 1) % lookaheadSamples;
    return out;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param buffer The input buffer
 * @param blockSize The size of the block
 */
void dibiff::gate::LookaheadGate::process() {
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
 * @brief Reset the lookahead gate
 * @details Resets the envelope and delay buffer
 */
void dibiff::gate::LookaheadGate::reset() {
    envelope = 0.0f;
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
}
/**
 * @brief Clear the lookahead gate
 * @details Clears the delay buffer
 */
void dibiff::gate::LookaheadGate::clear() {
    reset();
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::gate::LookaheadGate::getInput() { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::gate::LookaheadGate::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::gate::LookaheadGate::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the lookahead gate is finished processing
 * @return True if the lookahead gate is finished processing, false otherwise
 */
bool dibiff::gate::LookaheadGate::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::gate::LookaheadGate::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new lookahead gate object
 * @param threshold The threshold level in dB
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param lookaheadTime The lookahead time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::gate::LookaheadGate> dibiff::gate::LookaheadGate::create(float threshold, float attackTime, float releaseTime, float lookaheadTime, float sampleRate) {
    auto instance = std::make_shared<dibiff::gate::LookaheadGate>(threshold, attackTime, releaseTime, lookaheadTime, sampleRate);
    instance->initialize();
    return instance;
}