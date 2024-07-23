/// Limiter.cpp

#include "Limiter.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::dynamic::Limiter::getName() const { return "Limiter"; }
/**
 * @brief Constructor
 * @details Initializes the limiter with default values
 * @param threshold The threshold of the limiter in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the limiter in seconds, default value is 0.01
 * @param release The release time of the limiter in seconds, default value is 0.1
 * @param makeupGain The makeup gain of the limiter in dB, default value is calculated
 * @param kneeWidth The knee width of the limiter in dB, default value is calculated
 * 
 */
dibiff::dynamic::Limiter::Limiter(float threshold, float sampleRate, float attack = 0.01f, float release = 0.1f, std::optional<float> makeupGain = std::nullopt, std::optional<float> kneeWidth = std::nullopt)
: dibiff::graph::AudioObject(), 
    threshold(threshold), sampleRate(sampleRate), attack(attack), release(release), makeupGain(makeupGain), knee(kneeWidth) {};
/**
 * @brief Initialize
 * @details Initializes the limiter connection points and makeup gain
 */
void dibiff::dynamic::Limiter::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "LimiterInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "LimiterOutput"));
    /// If makeupGain is not specified, calculate it:
    if (!makeupGain) {
        makeupGain = 0.0f - calculateStaticCharacteristic(0.0f);
    }
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::dynamic::Limiter::process(float sample) {
    /// Convert to dB
    float inputdB = 20.0f * std::log10f(sample);
    /// Static Characteristic
    float xSc = calculateStaticCharacteristic(inputdB);
    /// Gain Smoothing
    updateGainSmoothing(xSc, inputdB);
    /// Makeup Gain
    float gM = gS * makeupGain.value();
    /// Convert back to linear scale
    float gLin = std::powf(10.0f, gM / 20.0f);
    /// Output
    return gLin * sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::dynamic::Limiter::process() {
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
 * @brief Calculates the static gain characteristic of the limiter
 * @details Calculates the static gain characteristic of the limiter
 * @param inputdB The input in dB
 * @return The output in dB
 */
float dibiff::dynamic::Limiter::calculateStaticCharacteristic(float inputdB) {
    if (knee) {
        /// Soft knee
        if (inputdB < (threshold - (knee.value() / 2.0f))) {
            return inputdB;
        } else if (inputdB > (threshold + (knee.value() / 2.0f))) {
            return threshold;
        } else {
            float numerator = std::powf(inputdB - threshold + (knee.value() / 2.0f), 2.0f);
            return inputdB - (numerator / (2.0f * knee.value()));
        } 
    } else {
        /// Hard knee
        if (inputdB < threshold) {
            return inputdB;
        } else {
            return threshold;
        }
    }
}
/**
 * @brief Calculates the gain smoothing of the limiter
 * @details Calculates the gain smoothing of the limiter
 * @param xSc The static characteristic of the limiter
 * @param inputdB The input in dB
 */
void dibiff::dynamic::Limiter::updateGainSmoothing(float xSc, float inputdB) {
    float gC = xSc - inputdB; /// Potentially just input?
    float alpha = [=]() -> float {
        if (gC <= gS) {
            return std::expf(-log10f(9.0f) / (attack * sampleRate));
        } else {
            return std::expf(-log10f(9.0f) / (release * sampleRate));
        }
    }();
    gS = alpha * gS + (1.0f - alpha) * gC;
}
/**
 * @brief Reset the limiter
 * @details Resets the gain smoothing
 */
void dibiff::dynamic::Limiter::reset() {
    gS = 0.0f;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::dynamic::Limiter::getInput() { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::dynamic::Limiter::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::dynamic::Limiter::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the limiter is finished processing
 * @return True if the limiter is finished processing, false otherwise
 */
bool dibiff::dynamic::Limiter::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::dynamic::Limiter::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new limiter object
 * @param threshold The threshold of the limiter in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the limiter in seconds, default value is 0.01
 * @param release The release time of the limiter in seconds, default value is 0.1
 * @param makeupGain The makeup gain of the limiter in dB, default value is calculated
 * @param kneeWidth The knee width of the limiter in dB, default value is calculated
 */
std::shared_ptr<dibiff::dynamic::Limiter> dibiff::dynamic::Limiter::create(float threshold, float sampleRate, float attack = 0.01f, float release = 0.1f, std::optional<float> makeupGain = std::nullopt, std::optional<float> kneeWidth = std::nullopt) {
    auto instance = std::make_shared<dibiff::dynamic::Limiter>(threshold, sampleRate, attack, release, makeupGain, kneeWidth);
    instance->initialize();
    return instance;
}