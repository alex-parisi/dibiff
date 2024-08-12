/// Reverb.cpp

#include "Reverb.h"
#include "../inc/Eigen/Dense"

#include <iostream>

/**
 * @brief Constructor
 * @details Initializes the reverb object with a certain decay time
 * and room size
 * @param decayTime The decay time of the reverb in seconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Reverb::Reverb(float& decayTime, float& roomSize, float& sampleRate, int& numDelays, float& wetLevel) 
: dibiff::graph::AudioObject(), decayTime(decayTime), roomSize(roomSize), sampleRate(sampleRate), numDelays(numDelays), wetLevel(wetLevel) {
    name = "Reverb";
};
/**
 * @brief Initialize
 * @details Initializes the reverb connection points and delay buffers
 */
void dibiff::effect::Reverb::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "ReverbInput"));
    _inputs.push_back(input);
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "ReverbOutput"));
    _outputs.push_back(output);
    // Initialize delay buffers
    int delayLength;
    for (int i = 0; i < numDelays; ++i) {
        delayLength = static_cast<int>((roomSize / speedOfSound) * sampleRate) * (i + 1);
        buffers.push_back(std::vector<float>(delayLength, 0.0f));
        bufferIndices.push_back(0);
    }
    feedback = std::pow(10, -3.0f * delayLength / (decayTime * sampleRate));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::effect::Reverb::process(float sample) {
    float out = 0.0f;
    const float alpha = 0.5f; // Interpolation factor, adjust as needed
    for (int i = 0; i < numDelays; ++i) {
        int index = bufferIndices[i];
        int prevIndex = (index - 1 + buffers[i].size()) % buffers[i].size();
        float delayedSample = buffers[i][index];
        float prevDelayedSample = buffers[i][prevIndex];
        // Linear interpolation between the current and previous delayed samples
        float interpolatedSample = (1.0f - alpha) * prevDelayedSample + alpha * delayedSample;
        out += interpolatedSample;
        // Update the buffer with the new sample
        buffers[i][index] = sample + feedback * delayedSample;
        // Move to the next index
        bufferIndices[i] = (index + 1) % buffers[i].size();
    }
    return out / numDelays;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Reverb::process() {
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> out(input->getBlockSize(), 0.0f);
        output->setData(out, input->getBlockSize());
        markProcessed();
    } else if (input->isReady()) {
        std::vector<float> audioData = *input->getData();
        int blockSize = input->getBlockSize();
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = audioData[i];
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
 * @brief Reset the reverb
 * @details Resets the delay buffers
 */
void dibiff::effect::Reverb::reset() {
    for (int i = 0; i < numDelays; ++i) {
        bufferIndices[i] = 0;
        std::fill(buffers[i].begin(), buffers[i].end(), 0.0f);
    }
}
/**
 * @brief Clear the reverb
 * @details Clears the delay buffers
 */
void dibiff::effect::Reverb::clear() {
    for (int i = 0; i < numDelays; ++i) {
        std::fill(buffers[i].begin(), buffers[i].end(), 0.0f);
    }
}
/**
 * @brief Check if the reverb is finished processing
 * @return True if the reverb is finished processing, false otherwise
 */
bool dibiff::effect::Reverb::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Reverb::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new reverb object
 * @param decayTime The decay time of the reverb in seconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 * @param numDelays The number of delay lines
 */
std::shared_ptr<dibiff::effect::Reverb> dibiff::effect::Reverb::create(float& decayTime, float& roomSize, float& sampleRate, int& numDelays, float& wetLevel) {
    auto instance = std::make_shared<dibiff::effect::Reverb>(decayTime, roomSize, sampleRate, numDelays, wetLevel);
    instance->initialize();
    return instance;
}