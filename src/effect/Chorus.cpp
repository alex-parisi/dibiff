/// Chorus.cpp

#include "Chorus.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the chorus object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the chorus in milliseconds
 * @param modulationRate The modulation rate of the chorus in Hz
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Chorus::Chorus(float modulationDepth, float modulationRate, float sampleRate, float wetLevel) 
: dibiff::graph::AudioObject(), modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate), wetLevel(wetLevel) {
    name = "Chorus";
};
/**
 * @brief Initialize
 * @details Initializes the chorus connection points and buffer
 */
void dibiff::effect::Chorus::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "ChorusInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "ChorusOutput"));
    maxDelaySamples = static_cast<int>(modulationDepth * sampleRate / 1000.0f);
    buffer.resize(maxDelaySamples, 0.0f);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::effect::Chorus::process(float sample) {
    // Calculate the current delay time using the LFO
    float lfo = (1.0f + std::sin(2.0f * M_PI * modulationRate * phase)) / 2.0f;
    float delaySamples = lfo * maxDelaySamples;
    // Calculate the indices for interpolation
    int intDelaySamples = static_cast<int>(delaySamples);
    float fracDelaySamples = delaySamples - intDelaySamples;
    int delayIndex1 = (bufferIndex - intDelaySamples + buffer.size()) % buffer.size();
    int delayIndex2 = (delayIndex1 - 1 + buffer.size()) % buffer.size();
    // Linear interpolation between two samples
    float delayedSample = buffer[delayIndex1] * (1.0f - fracDelaySamples) +
                          buffer[delayIndex2] * fracDelaySamples;
    // Update the buffer with the current sample
    buffer[bufferIndex] = sample;
    bufferIndex = (bufferIndex + 1) % buffer.size();
    // Increment the phase for the LFO
    phase += modulationRate / sampleRate;
    if (phase >= 1.0f) {
        phase -= 1.0f;
    }
    // Mix the input signal with the delayed signal
    return sample + delayedSample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Chorus::process() {
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
            out[i] = wetLevel * y(i) + (1.0f - wetLevel) * x(i);
        }
        output->setData(out, blockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the chorus
 * @details Resets the delay buffer
 */
void dibiff::effect::Chorus::reset() {
    buffer = std::vector<float>(buffer.size(), 0.0f);
}
/**
 * @brief Clear the chorus
 * @details Clears the delay buffer
 */
void dibiff::effect::Chorus::clear() {
    reset();
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Chorus::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Chorus::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Chorus::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the chorus is finished processing
 * @return True if the chorus is finished processing, false otherwise
 */
bool dibiff::effect::Chorus::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Chorus::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new chorus object
 * @param modulationDepth The modulation depth of the chorus in milliseconds
 * @param modulationRate The modulation rate of the chorus in Hz
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::effect::Chorus> dibiff::effect::Chorus::create(float modulationDepth, float modulationRate, float sampleRate, float wetLevel) {
    auto instance = std::make_shared<dibiff::effect::Chorus>(modulationDepth, modulationRate, sampleRate, wetLevel);
    instance->initialize();
    return instance;
}