/// Flanger.cpp

#include "Flanger.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the flanger object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the flanger in milliseconds
 * @param modulationRate The modulation rate of the flanger in Hz
 * @param sampleRate The sample rate of the input signal
 * @param feedback The feedback amount of the flanger effect
 */
dibiff::effect::Flanger::Flanger(float& modulationDepth, float& modulationRate, float& sampleRate, float& feedback, float& wetLevel) 
: dibiff::graph::AudioObject(), modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate), feedback(feedback), wetLevel(wetLevel) {
    name = "Flanger";
};
/**
 * @brief Initialize
 * @details Initializes the flanger connection points and buffer
 */
void dibiff::effect::Flanger::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "FlangerInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "FlangerOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
    maxDelaySamples = static_cast<int>(modulationDepth * sampleRate / 1000.0f);
    buffer.resize(maxDelaySamples, 0.0f);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::effect::Flanger::process(float sample) {
    // Calculate the current delay time
    float lfo = (1.0f + std::sin(2.0f * M_PI * modulationRate * phase)) / 2.0f;
    int delaySamples = static_cast<int>(lfo * maxDelaySamples);
    // Get the delayed sample
    int delayIndex = (bufferIndex - delaySamples + buffer.size()) % buffer.size();
    float delayedSample = buffer[delayIndex];
    // Update the buffer
    buffer[bufferIndex] = sample + feedback * delayedSample;
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
void dibiff::effect::Flanger::process() {
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> out(input->getBlockSize(), 0.0f);
        output->setData(out, input->getBlockSize());
        markProcessed();
    } else if (input->isReady()) {
        const std::vector<float>& data = input->getData();
        const int blockSize = input->getBlockSize();
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
 * @brief Reset the flanger
 * @details Resets the delay buffer
 */
void dibiff::effect::Flanger::reset() {
    buffer = std::vector<float>(buffer.size(), 0.0f);
}
/**
 * @brief Clear the flanger
 * @details Clears the delay buffer
 */
void dibiff::effect::Flanger::clear() {
    reset();
}
/**
 * @brief Check if the flanger is finished processing
 * @return True if the flanger is finished processing, false otherwise
 */
bool dibiff::effect::Flanger::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Flanger::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new flanger object
 * @param modulationDepth The modulation depth of the flanger in milliseconds
 * @param modulationRate The modulation rate of the flanger in Hz
 * @param sampleRate The sample rate of the input signal
 * @param feedback The feedback amount of the flanger effect
 */
std::unique_ptr<dibiff::effect::Flanger> dibiff::effect::Flanger::create(float& modulationDepth, float& modulationRate, float& sampleRate, float& feedback, float& wetLevel) {
    auto instance = std::make_unique<dibiff::effect::Flanger>(modulationDepth, modulationRate, sampleRate, feedback, wetLevel);
    instance->initialize();
    return std::move(instance);
}