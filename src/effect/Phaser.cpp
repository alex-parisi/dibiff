/// Phaser.cpp

#include "Phaser.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the phaser object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the phaser in Hz
 * @param modulationRate The modulation rate of the phaser in Hz
 * @param sampleRate The sample rate of the input signal
 * @param baseCutoff The base cutoff frequency of the phaser, default value is 1000.0 Hz
 * @param mix The mix of the phaser effect, default value is 0.5
 * @param numStages The number of all-pass filter in the phaser, default value is 4
 */
dibiff::effect::Phaser::Phaser(float& modulationDepth, float& modulationRate, float& sampleRate, float& baseCutoff, float& mix, int& numStages)
: dibiff::graph::AudioObject(), modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate), baseCutoff(baseCutoff), mix(mix), numStages(numStages) {
    name = "Phaser";
};
/**
 * @brief Initialize
 * @details Initializes the phaser connection points and all-pass filters
 */
void dibiff::effect::Phaser::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "PhaserInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "PhaserOutput"));
    // Initialize all-pass filter
    for (int i = 0; i < numStages; ++i) {
        float initialCutoff = baseCutoff + (i * modulationDepth / numStages);
        float qFactor = 0.7071067811865476f;
        dibiff::filter::AllPassFilter apf(initialCutoff, sampleRate, qFactor);
        apf.initialize();
        allPassFilters.emplace_back(apf);
    }
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::effect::Phaser::process(float sample) {
    // Calculate the current modulation factor
    float lfo = (1.0f + std::sin(2.0f * M_PI * modulationRate * phase)) / 2.0f;
    float currentModulation = baseCutoff + lfo * modulationDepth;
    // Apply modulation to each all-pass filter
    for (auto& filter : allPassFilters) {
        filter.setCutoff(currentModulation);
    }
    // Process the input sample through each all-pass filter
    float out = sample;
    for (auto& filter : allPassFilters) {
        out = filter.process(out);
    }
    // Increment the phase for the LFO
    phase += 1.0f / sampleRate;
    if (phase >= 1.0f) {
        phase -= 1.0f;
    }
    // Mix the input signal with the processed signal
    return sample + out * mix;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Phaser::process() {
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
            out[i] = y(i);
        }
        output->setData(out, blockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the phaser
 * @details Resets the all-pass filter
 */
void dibiff::effect::Phaser::reset() {
    for (auto& filter : allPassFilters) {
        filter.reset();
    }
}
/**
 * @brief Clear the phaser
 * @details Clears the all-pass filter
 */
void dibiff::effect::Phaser::clear() {
    for (auto& filter : allPassFilters) {
        filter.clear();
    }
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Phaser::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Phaser::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Phaser::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the phaser is finished processing
 * @return True if the phaser is finished processing, false otherwise
 */
bool dibiff::effect::Phaser::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Phaser::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new phaser object
 * @param modulationDepth The modulation depth of the phaser in milliseconds
 * @param modulationRate The modulation rate of the phaser in Hz
 * @param sampleRate The sample rate of the input signal
 * @param baseCutoff The base cutoff frequency of the phaser, default value is 1000.0 Hz
 * @param mix The mix of the phaser effect, default value is 0.5
 * @param numStages The number of all-pass filter in the phaser, default value is 4
 */
std::shared_ptr<dibiff::effect::Phaser> dibiff::effect::Phaser::create(float& modulationDepth, float& modulationRate, float& sampleRate, float& baseCutoff, float& mix, int& numStages) {
    auto instance = std::make_shared<dibiff::effect::Phaser>(modulationDepth, modulationRate, sampleRate, baseCutoff, mix, numStages);
    instance->initialize();
    return instance;
}