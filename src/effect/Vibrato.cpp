/// Vibrato.cpp

#include "Vibrato.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::effect::Vibrato::getName() const { return "Vibrato"; }
/**
 * @brief Constructor
 * @details Initializes the vibrato object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the vibrato in milliseconds
 * @param modulationRate The modulation rate of the vibrato in Hz
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Vibrato::Vibrato(float modulationDepth, float modulationRate, float sampleRate)
: dibiff::graph::AudioObject(), 
    modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the vibrato connection points and buffer
 */
void dibiff::effect::Vibrato::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "VibratoInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "VibratoOutput"));
    maxDelaySamples = static_cast<int>(modulationDepth * sampleRate / 1000.0f);
    buffer.resize(maxDelaySamples, 0.0f);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 */
float dibiff::effect::Vibrato::process(float sample) {
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
    // Return the delayed sample (pitch-modulated signal)
    return delayedSample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Vibrato::process() {
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
 * @brief Reset the vibrato
 * @details Resets the delay buffer
 */
void dibiff::effect::Vibrato::reset() {
    buffer = std::vector<float>(buffer.size(), 0.0f);
}
/**
 * @brief Clear the vibrato
 * @details Clears the delay buffer
 */
void dibiff::effect::Vibrato::clear() {
    reset();
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Vibrato::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Vibrato::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Vibrato::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the vibrato is finished processing
 * @return True if the vibrato is finished processing, false otherwise
 */
bool dibiff::effect::Vibrato::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Vibrato::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new vibrato object
 * @param modulationDepth The modulation depth of the vibrato in milliseconds
 * @param modulationRate The modulation rate of the vibrato in Hz
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::effect::Vibrato> dibiff::effect::Vibrato::create(float modulationDepth, float modulationRate, float sampleRate) {
    auto instance = std::make_shared<dibiff::effect::Vibrato>(modulationDepth, modulationRate, sampleRate);
    instance->initialize();
    return instance;
}