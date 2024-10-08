/// AutomaticGainControl.cpp

#include "AutomaticGainControl.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the AGC with given parameters
 * @param targetLevel The target output level in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the AGC in seconds, default value is 0.01
 * @param release The release time of the AGC in seconds, default value is 0.1
 * @param rmsCoefficient The coefficient for RMS level calculation, default value is 0.999
 */
dibiff::level::AutomaticGainControl::AutomaticGainControl(float& targetLevel, float& sampleRate, float& attack, float& release, float& rmsCoefficient)
: dibiff::graph::AudioObject(), 
  targetLevel(targetLevel), sampleRate(sampleRate), attack(attack), release(release), rmsCoefficient(rmsCoefficient) {
    name = "AutomaticGainControl";
};
/**
 * @brief Initialize
 * @details Initializes the AGC connection points and parameters
 */
void dibiff::level::AutomaticGainControl::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "AutomaticGainControlInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "AutomaticGainControlOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
    attackCoefficient = std::exp(-1.0f / (attack * sampleRate));
    releaseCoefficient = std::exp(-1.0f / (release * sampleRate));
    targetLevelLinear = std::pow(10.0f, targetLevel / 20.0f);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::level::AutomaticGainControl::process(float sample) {
    float inputLevel = std::fabs(sample);
    // Calculate RMS level
    rmsLevel = rmsCoefficient * rmsLevel + (1.0f - rmsCoefficient) * inputLevel * inputLevel;
    float rmsValue = std::sqrt(rmsLevel);
    // Gain adjustment
    float desiredGain = targetLevelLinear / (rmsValue + 1e-6f); // Avoid division by zero
    if (desiredGain < currentGain) {
        currentGain = attackCoefficient * currentGain + (1.0f - attackCoefficient) * desiredGain;
    } else {
        currentGain = releaseCoefficient * currentGain + (1.0f - releaseCoefficient) * desiredGain;
    }
    // Apply gain
    return sample * currentGain;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::level::AutomaticGainControl::process() {
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
            out[i] = y(i);
        }
        output->setData(out, blockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the AGC
 * @details Resets the AGC to the default state
 */
void dibiff::level::AutomaticGainControl::reset() {
    currentGain = 1.0f;
    rmsLevel = 0.0f;
}
/**
 * @brief Check if the AGC is finished processing
 * @return True if the AGC is finished processing, false otherwise
 */
bool dibiff::level::AutomaticGainControl::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::level::AutomaticGainControl::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new AGC object
 * @param targetLevel The target output level in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the AGC in seconds, default value is 0.01
 * @param release The release time of the AGC in seconds, default value is 0.1
 * @param rmsCoefficient The coefficient for RMS level calculation, default value is 0.999
 */
std::unique_ptr<dibiff::level::AutomaticGainControl> dibiff::level::AutomaticGainControl::create(float& targetLevel, float& sampleRate, float& attack, float& release, float& rmsCoefficient) {
    auto instance = std::make_unique<dibiff::level::AutomaticGainControl>(targetLevel, sampleRate, attack, release, rmsCoefficient);
    instance->initialize();
    return std::move(instance);
}