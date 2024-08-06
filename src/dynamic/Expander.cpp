/// Expander.cpp

#include "Expander.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the expander with default values
 * @param threshold The threshold of the expander in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the expander in seconds, default value is 0.01
 * @param release The release time of the expander in seconds, default value is 0.1
 * @param ratio The ratio of the expander, default value is 2.0
 * @param kneeWidth The knee width of the expander in dB, default value is calculated
 */
dibiff::dynamic::Expander::Expander(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> kneeWidth)
: dibiff::graph::AudioObject(), 
  threshold(threshold), sampleRate(sampleRate), attack(attack), release(release), ratio(ratio), knee(kneeWidth) {
    name = "Expander";
};
/**
 * @brief Initialize
 * @details Initializes the expander connection points
 */
void dibiff::dynamic::Expander::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "ExpanderInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "ExpanderOutput"));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::dynamic::Expander::process(float sample) {
    /// Convert to dB
    float inputdB = 20.0f * std::log10(std::fabs(sample));
    /// Static Characteristic
    float xSc = calculateStaticCharacteristic(inputdB);
    /// Gain Smoothing
    updateGainSmoothing(xSc, inputdB);
    /// Convert back to linear scale
    float gLin = std::pow(10.0f, gS / 20.0f);
    /// Output
    return gLin * sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::dynamic::Expander::process() {
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
 * @brief Calculates the static gain characteristic of the expander
 * @details Calculates the static gain characteristic of the expander
 * @param inputdB The input in dB
 * @return The output in dB
 */
float dibiff::dynamic::Expander::calculateStaticCharacteristic(float inputdB) {
    float expanderThreshold = threshold + ((inputdB - threshold) * (1.0f / ratio));
    if (knee) {
        /// Soft knee
        if (inputdB > (expanderThreshold + (knee.value() / 2.0f))) {
            return inputdB;
        } else if (inputdB < (expanderThreshold - (knee.value() / 2.0f))) {
            return expanderThreshold;
        } else {
            float numerator = std::pow(inputdB - expanderThreshold - (knee.value() / 2.0f), 2.0f);
            return inputdB + (numerator / (2.0f * knee.value()));
        } 
    } else {
        /// Hard knee
        if (inputdB > expanderThreshold) {
            return inputdB;
        } else {
            return expanderThreshold;
        }
    }
}
/**
 * @brief Updates the gain smoothing of the expander
 * @details Updates the gain smoothing of the expander
 * @param xSc The static characteristic of the expander
 * @param inputdB The input in dB
 */
void dibiff::dynamic::Expander::updateGainSmoothing(float xSc, float inputdB) {
    float gC = xSc - inputdB; /// Gain change
    float alpha = [=]() -> float {
        if (gC >= gS) {
            return std::exp(-std::log10(9.0f) / (attack * sampleRate));
        } else {
            return std::exp(-std::log10(9.0f) / (release * sampleRate));
        }
    }();
    gS = alpha * gS + (1.0f - alpha) * gC;
}
/**
 * @brief Reset the expander
 * @details Resets the gain smoothing
 */
void dibiff::dynamic::Expander::reset() {
    gS = 0.0f;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::dynamic::Expander::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::dynamic::Expander::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::dynamic::Expander::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the expander is finished processing
 * @return True if the expander is finished processing, false otherwise
 */
bool dibiff::dynamic::Expander::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::dynamic::Expander::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new expander object
 * @param threshold The threshold of the expander in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the expander in seconds, default value is 0.01
 * @param release The release time of the expander in seconds, default value is 0.1
 * @param ratio The ratio of the expander, default value is 2.0
 * @param kneeWidth The knee width of the expander in dB, default value is calculated
 */
std::shared_ptr<dibiff::dynamic::Expander> dibiff::dynamic::Expander::create(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> kneeWidth) {
    auto instance = std::make_shared<dibiff::dynamic::Expander>(threshold, sampleRate, attack, release, ratio, kneeWidth);
    instance->initialize();
    return instance;
}