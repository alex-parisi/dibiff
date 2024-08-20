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
        auto in = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "MixerInput" + std::to_string(i)));
        _inputs.emplace_back(std::move(in));
    }
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "MixerOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::level::Mixer::process() {
    std::vector<bool> connected(numInputs, false);
    for (int i = 0; i < numInputs; ++i) {
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
        connected[i] = in->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < numInputs; ++i) {
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
        if (connected[i] && !in->isReady()) {
            isReady = false;
            break;
        }
    }
    if (isReady) {
        auto in0 = static_cast<dibiff::graph::AudioInput*>(_inputs[0].get());
        const int blockSize = in0->getBlockSize();
        Eigen::VectorXf y(blockSize);
        y.setZero();
        for (int i = 0; i < numInputs; ++i) {
            auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
            if (connected[i]) {
                const std::vector<float>& data = in->getData();
                for (int j = 0; j < blockSize; ++j) {
                    y(j) += data[j] / numInputs;
                }
            } else {
                for (int j = 0; j < blockSize; ++j) {
                    y(j) += 0;
                }
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
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
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
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
        connected[i] = in->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < numInputs; ++i) {
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
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
std::unique_ptr<dibiff::level::Mixer> dibiff::level::Mixer::create(int numInputs) {
    auto instance = std::make_unique<dibiff::level::Mixer>(numInputs);
    instance->initialize();
    return std::move(instance);
}