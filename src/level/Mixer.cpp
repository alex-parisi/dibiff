/// Mixer.cpp

#include "Mixer.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::level::Mixer::getName() const { return "Mixer"; }
/**
 * @brief Constructor
 * @details Initializes the mixer object with a certain value
 * @param value The mixer of the object in dB
 */
dibiff::level::Mixer::Mixer(int numInputs)
: dibiff::graph::AudioObject(), numInputs(numInputs) {};
/**
 * @brief Initialize
 * @details Initializes the mixer connection points
 */
void dibiff::level::Mixer::initialize() {
    for (int i = 0; i < numInputs; ++i) {
        inputs.push_back(std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "MixerInput" + std::to_string(i))));
    }
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "MixerOutput"));
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::level::Mixer::process() {
    /// Check if each input in inputs is ready:
    bool allReady = true;
    for (int i = 0; i < numInputs; ++i) {
        if (!inputs[i]->isReady()) {
            allReady = false;
            break;
        }
    }
    if (allReady) {
        int blockSize = inputs[0]->getBlockSize();
        Eigen::VectorXf y(blockSize);
        y.setZero();
        for (int i = 0; i < numInputs; ++i) {
            std::vector<float> data = *inputs[i]->getData();
            for (int j = 0; j < blockSize; ++j) {
                y(j) += data[j] / numInputs;
            }
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
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::level::Mixer::getInput(int i) { return inputs[i]; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::level::Mixer::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::level::Mixer::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the mixer is finished processing
 * @return True if the mixer is finished processing, false otherwise
 */
bool dibiff::level::Mixer::isFinished() const {
    bool allFinished = true;
    for (int i = 0; i < numInputs; ++i) {
        if (!inputs[i]->isConnected() || !inputs[i]->isReady() || !inputs[i]->isFinished()) {
            allFinished = false;
            break;
        }
    }
    return allFinished && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::level::Mixer::isReadyToProcess() const {
    bool allReady = true;
    for (int i = 0; i < numInputs; ++i) {
        if (!inputs[i]->isConnected() || !inputs[i]->isReady()) {
            allReady = false;
            break;
        }
    }
    return allReady && !processed;
}
/**
 * Create a new mixer object
 * @param value The mixer of the object in dB
 */
std::shared_ptr<dibiff::level::Mixer> dibiff::level::Mixer::create(int numInputs) {
    auto instance = std::make_shared<dibiff::level::Mixer>(numInputs);
    instance->initialize();
    return instance;
}