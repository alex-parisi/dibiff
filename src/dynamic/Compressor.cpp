/// Compressor.cpp

#include "Compressor.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the compressor with default values
 * @param threshold The threshold of the compressor in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the compressor in seconds, default value is 0.01
 * @param release The release time of the compressor in seconds, default value is 0.1
 * @param ratio The ratio of the compressor, default value is 2.0
 * @param makeupGain The makeup gain of the compressor in dB, default value is calculated
 * @param kneeWidth The knee width of the compressor in dB, default value is calculated
 * 
 */
dibiff::dynamic::Compressor::Compressor(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> makeupGain, std::optional<std::reference_wrapper<float>> kneeWidth)
: dibiff::graph::AudioObject(), 
  threshold(threshold), sampleRate(sampleRate), attack(attack), release(release), ratio(ratio), makeupGain(makeupGain), knee(kneeWidth) {
    name = "Compressor";
};
/**
 * @brief Initialize
 * @details Initializes the compressor connection points and makeup gain
 */
void dibiff::dynamic::Compressor::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "CompressorInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "CompressorOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
    /// If makeupGain is not specified, calculate it:
    if (!makeupGain) {
        _makeupGain = 0.0f - calculateStaticCharacteristic(0.0f);
    } else {
        _makeupGain = makeupGain.value();
    }
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::dynamic::Compressor::process(float sample) {
    /// Convert to dB
    float inputdB = 20.0f * std::log10(sample);
    /// Static Characteristic
    float xSc = calculateStaticCharacteristic(inputdB);
    /// Gain Smoothing
    updateGainSmoothing(xSc, inputdB);
    /// Makeup Gain
    float gM = gS * _makeupGain;
    /// Convert back to linear scale
    float gLin = std::pow(10.0f, gM / 20.0f);
    /// Output
    return gLin * sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::dynamic::Compressor::process() {
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        const std::vector<float>& inData = input->getData();
        const int inBlockSize = input->getBlockSize();
        std::vector<float> out(inBlockSize, 0.0f);
        output->setData(out, inBlockSize);
    } else if (input->isReady()) {
        /// If connected and ready, process the data
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
 * @brief Calculates the static gain characteristic of the compressor
 * @details Calculates the static gain characteristic of the compressor
 * @param inputdB The input in dB
 * @return The output in dB
 */
float dibiff::dynamic::Compressor::calculateStaticCharacteristic(float inputdB) {
    float compressorThreshold = threshold - ((threshold - inputdB) / ratio);
    if (knee) {
        /// Soft knee
        if (inputdB < (compressorThreshold - (knee.value() / 2.0f))) {
            return inputdB;
        } else if (inputdB > (compressorThreshold + (knee.value() / 2.0f))) {
            return compressorThreshold;
        } else {
            float numerator = std::pow(inputdB - compressorThreshold + (knee.value() / 2.0f), 2.0f);
            return inputdB - (numerator / (2.0f * knee.value()));
        } 
    } else {
        /// Hard knee
        if (inputdB < compressorThreshold) {
            return inputdB;
        } else {
            return compressorThreshold;
        }
    }
}
/**
 * @brief Calculates the gain smoothing of the compressor
 * @details Calculates the gain smoothing of the compressor
 * @param xSc The static characteristic of the compressor
 * @param inputdB The input in dB
 */
void dibiff::dynamic::Compressor::updateGainSmoothing(float xSc, float inputdB) {
    float gC = xSc - inputdB; /// Potentially just input?
    float alpha = [=]() -> float {
        if (gC <= gS) {
            return std::exp(-log10(9.0f) / (attack * sampleRate));
        } else {
            return std::exp(-log10(9.0f) / (release * sampleRate));
        }
    }();
    float gS = alpha * gS + (1.0f - alpha) * gC;
}
/**
 * @brief Reset the compressor
 * @details Resets the gain smoothing
 */
void dibiff::dynamic::Compressor::reset() {
    gS = 0.0f;
}
/**
 * @brief Check if the compressor is finished processing
 * @return True if the compressor is finished processing, false otherwise
 */
bool dibiff::dynamic::Compressor::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::dynamic::Compressor::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new compressor object
 * @param threshold The threshold of the compressor in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the compressor in seconds, default value is 0.01
 * @param release The release time of the compressor in seconds, default value is 0.1
 * @param ratio The ratio of the compressor, default value is 2.0
 * @param makeupGain The makeup gain of the compressor in dB, default value is calculated
 * @param kneeWidth The knee width of the compressor in dB, default value is calculated
 */
std::unique_ptr<dibiff::dynamic::Compressor> dibiff::dynamic::Compressor::create(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> makeupGain, std::optional<std::reference_wrapper<float>> kneeWidth) {
    auto instance = std::make_unique<dibiff::dynamic::Compressor>(threshold, sampleRate, attack, release, ratio, makeupGain, kneeWidth);
    instance->initialize();
    return std::move(instance);
}