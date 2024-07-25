/// Reverb.cpp

#include "Reverb.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::effect::Reverb::getName() const { return "Reverb"; }
/**
 * @brief Constructor
 * @details Initializes the reverb object with a certain decay time
 * and room size
 * @param decayTime The decay time of the reverb in seconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Reverb::Reverb(float decayTime, float roomSize, float sampleRate, int numDelays) 
: dibiff::graph::AudioObject(), 
    decayTime(decayTime), roomSize(roomSize), sampleRate(sampleRate), numDelays(numDelays) {};
/**
 * @brief Initialize
 * @details Initializes the reverb connection points and delay buffers
 */
void dibiff::effect::Reverb::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "ReverbInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "ReverbOutput"));
    // Initialize delay buffers
    int delayLength;
    for (int i = 0; i < numDelays; ++i) {
        delayLength = static_cast<int>((roomSize / speedOfSound) * sampleRate) * (i + 1);
        buffers.emplace_back(std::vector<float>(delayLength, 0.0f));
        bufferIndices.push_back(0);
    }
    feedback = std::powf(10, -3.0f * delayLength / (decayTime * sampleRate));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::effect::Reverb::process(float sample) {
    float output = 0.0f;
    for (int i = 0; i < numDelays; ++i) {
        int index = bufferIndices[i];
        float delayedSample = buffers[i][index];
        output += delayedSample;
        buffers[i][index] = sample + feedback * delayedSample;
        bufferIndices[i] = (index + 1) % buffers[i].size();
    }
    return output / numDelays;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Reverb::process() {
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
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Reverb::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Reverb::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Reverb::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
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
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new reverb object
 * @param decayTime The decay time of the reverb in seconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 * @param numDelays The number of delay lines
 */
std::shared_ptr<dibiff::effect::Reverb> dibiff::effect::Reverb::create(float decayTime, float roomSize, float sampleRate, int numDelays) {
    auto instance = std::make_shared<dibiff::effect::Reverb>(decayTime, roomSize, sampleRate, numDelays);
    instance->initialize();
    return instance;
}