/// Chorus.cpp

#include "Chorus.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::effect::Chorus::getName() const { return "Chorus"; }
/**
 * @brief Constructor
 * @details Initializes the chorus object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the chorus in milliseconds
 * @param modulationRate The modulation rate of the chorus in Hz
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Chorus::Chorus(float modulationDepth, float modulationRate, float sampleRate) 
: dibiff::graph::AudioObject(), 
    modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate) {};
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
    // Calculate the current delay time
    float lfo = (1.0f + std::sinf(2.0f * M_PI * modulationRate * phase)) / 2.0f;
    int delaySamples = static_cast<int>(lfo * maxDelaySamples);
    // Get the delayed sample
    int delayIndex = (bufferIndex - delaySamples + buffer.size()) % buffer.size();
    float delayedSample = buffer[delayIndex];
    // Update the buffer
    buffer[bufferIndex] = sample;
    bufferIndex = (bufferIndex + 1) % buffer.size();
    // Increment the phase for the LFO
    phase += 1.0f / sampleRate;
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
std::weak_ptr<dibiff::graph::AudioInput> dibiff::effect::Chorus::getInput() { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::effect::Chorus::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::effect::Chorus::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
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
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new chorus object
 * @param modulationDepth The modulation depth of the chorus in milliseconds
 * @param modulationRate The modulation rate of the chorus in Hz
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::effect::Chorus> dibiff::effect::Chorus::create(float modulationDepth, float modulationRate, float sampleRate) {
    auto instance = std::make_shared<dibiff::effect::Chorus>(modulationDepth, modulationRate, sampleRate);
    instance->initialize();
    return instance;
}