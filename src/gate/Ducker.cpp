/// Ducker.cpp

#include "Ducker.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the ducker object with a certain threshold,
 * ratio, attack time, release time, and sample rate
 * @param threshold The threshold level in dB for the sidechain input
 * @param ratio The attenuation ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::gate::Ducker::Ducker(float& threshold, float& ratio, float& attackTime, float& releaseTime, float& sampleRate)
: dibiff::graph::AudioObject(), 
  _threshold(threshold), _ratio(ratio), _attackTime(attackTime), _releaseTime(releaseTime), _sampleRate(sampleRate) {
    name = "Ducker";
};
/**
 * @brief Initialize
 * @details Initializes the ducker connection points and envelope
 */
void dibiff::gate::Ducker::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "DuckerInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto r = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "DuckerReference"));
    _inputs.emplace_back(std::move(r));
    reference = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "DuckerOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
    _attackCoefficient = std::exp(-1.0f / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = std::exp(-1.0f / (_releaseTime * _sampleRate / 1000.0f));
    _thresholdLevel = std::pow(10.0f, _threshold / 20.0f); // Convert dB to linear
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The main input sample
 * @param sidechain The sidechain input sample
 */
float dibiff::gate::Ducker::process(float sample, float reference) {
    float sidechainLevel = std::fabs(reference);
    if (sidechainLevel > _thresholdLevel) {
        _envelope = _attackCoefficient * (_envelope - sidechainLevel) + sidechainLevel;
    } else {
        _envelope = _releaseCoefficient * _envelope;
    }
    if (sidechainLevel > _thresholdLevel) {
        float gainReduction = std::pow(_envelope / _thresholdLevel, 1.0f - _ratio);
        return sample * gainReduction;
    }
    return sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::gate::Ducker::process() {
    _attackCoefficient = std::exp(-1.0f / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = std::exp(-1.0f / (_releaseTime * _sampleRate / 1000.0f));
    _thresholdLevel = std::pow(10.0f, _threshold / 20.0f);
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        const std::vector<float>& inData = input->getData();
        const int inBlockSize = input->getBlockSize();
        std::vector<float> out(inBlockSize, 0.0f);
        output->setData(out, inBlockSize);
    } else if (!reference->isConnected()) {
        /// If no reference is connected, just pass the input through
        const std::vector<float>& inData = input->getData();
        const int inBlockSize = input->getBlockSize();
        output->setData(inData, inBlockSize);
    } else if (input->isReady() && reference->isReady()) {
        const std::vector<float>& inData = input->getData();
        const std::vector<float>& refData = reference->getData();
        const int inBlockSize = input->getBlockSize();
        const int refBlockSize = reference->getBlockSize();
        if (inBlockSize != refBlockSize) {
            throw std::runtime_error("Block sizes do not match");
        }
        Eigen::VectorXf x(inBlockSize), r(inBlockSize), y(inBlockSize);
        for (int i = 0; i < inBlockSize; ++i) {
            x(i) = inData[i];
            r(i) = refData[i];
        }
        for (int i = 0; i < inBlockSize; ++i) {
            y(i) = process(x(i), r(i));
        }
        std::vector<float> out(inBlockSize);
        for (int i = 0; i < inBlockSize; ++i) {
            out[i] = y(i);
        }
        output->setData(out, inBlockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the ducker
 * @details Resets the envelope to zero
 */
void dibiff::gate::Ducker::reset() {
    _envelope = 0.0f;
}
/**
 * @brief Check if the ducker is finished processing
 * @return True if the ducker is finished processing, false otherwise
 */
bool dibiff::gate::Ducker::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && reference->isConnected() && reference->isReady() && reference->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::gate::Ducker::isReadyToProcess() const {
    if (!reference->isConnected()) {
        return input->isConnected() && input->isReady() && !processed;
    } else if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && reference->isReady() && !processed;
}
/**
 * Create a new ducker object
 * @param threshold The threshold level in dB
 * @param ratio The attenuation ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::gate::Ducker> dibiff::gate::Ducker::create(float& threshold, float& ratio, float& attackTime, float& releaseTime, float& sampleRate) {
    auto instance = std::make_unique<dibiff::gate::Ducker>(threshold, ratio, attackTime, releaseTime, sampleRate);
    instance->initialize();
    return std::move(instance);
}