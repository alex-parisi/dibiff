/// Mixer.cpp

#include "Mixer.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the mixer object with a certain value
 * @param value The mixer of the object in dB
 */
dibiff::level::Mixer::Mixer(int numInputs)
: dibiff::graph::AudioObject(), numInputs(numInputs) {
    name = "Mixer";
};
/**
 * @brief Initialize
 * @details Initializes the mixer connection points
 */
void dibiff::level::Mixer::initialize() {
    for (int i = 0; i < numInputs; ++i) {
        _inputs.push_back(std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "MixerInput" + std::to_string(i))));
    }
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "MixerOutput"));
    _outputs.push_back(output);
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::level::Mixer::process() {
    std::vector<bool> connected(numInputs, false);
    for (int i = 0; i < numInputs; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        connected[i] = in->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < numInputs; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        if (connected[i] && !in->isReady()) {
            isReady = false;
            break;
        }
    }
    if (isReady) {
        auto in0 = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[0]);
        int blockSize = in0->getBlockSize();
        Eigen::VectorXf y(blockSize);
        y.setZero();
        for (int i = 0; i < numInputs; ++i) {
            auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
            std::vector<float> data;
            if (connected[i]) {
                data = *in->getData();
            } else {
                data = std::vector<float>(blockSize, 0.0f);
            }
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
 * @brief Check if the mixer is finished processing
 * @return True if the mixer is finished processing, false otherwise
 */
bool dibiff::level::Mixer::isFinished() const {
    bool allFinished = true;
    for (int i = 0; i < numInputs; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        if (!in->isConnected() || !in->isReady() || !in->isFinished()) {
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
    std::vector<bool> connected(numInputs, false);
    for (int i = 0; i < numInputs; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        connected[i] = in->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < numInputs; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        if (connected[i] && !in->isReady()) {
            isReady = false;
            break;
        }
    }
    return isReady && !processed;
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