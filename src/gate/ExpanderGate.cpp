/// ExpanderGate.cpp

#include "ExpanderGate.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the expander gate object with a certain threshold,
 * ratio, attack time, release time, and sample rate
 * @param threshold The threshold level in dB
 * @param ratio The expansion ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::gate::ExpanderGate::ExpanderGate(float& threshold, float& ratio, float& attackTime, float& releaseTime, float& sampleRate)
: dibiff::graph::AudioObject(), 
  _threshold(threshold), _ratio(ratio), _attackTime(attackTime), _releaseTime(releaseTime), _sampleRate(sampleRate) {
    name = "ExpanderGate";
};
/**
 * @brief Initialize
 * @details Initializes the expander gate connection points and envelope
 */
void dibiff::gate::ExpanderGate::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "ExpanderGateInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "ExpanderGateOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
    _attackCoefficient = exp(-1.0 / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = exp(-1.0 / (_releaseTime * _sampleRate / 1000.0f));
    _thresholdLevel = pow(10.0f, _threshold / 20.0f); // Convert dB to linear
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 * @return The output sample
 */
float dibiff::gate::ExpanderGate::process(float sample) {
    float inputLevel = fabs(sample);
    if (inputLevel > _thresholdLevel) {
        _envelope = _attackCoefficient * (_envelope - inputLevel) + inputLevel;
    } else {
        _envelope = _releaseCoefficient * _envelope;
    }
    if (inputLevel < _thresholdLevel) {
        float gainReduction = pow(_envelope / _thresholdLevel, _ratio - 1.0f);
        return sample * gainReduction;
    }
    return sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param buffer The input buffer
 * @param blockSize The size of the block
 */
void dibiff::gate::ExpanderGate::process() {
    _attackCoefficient = exp(-1.0 / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = exp(-1.0 / (_releaseTime * _sampleRate / 1000.0f));
    _thresholdLevel = pow(10.0f, _threshold / 20.0f);
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
 * @brief Reset the expander gate
 * @details Resets the envelope to zero
 */
void dibiff::gate::ExpanderGate::reset() {
    _envelope = 0.0f;
}
/**
 * @brief Check if the expander gate is finished processing
 * @return True if the expander gate is finished processing, false otherwise
 */
bool dibiff::gate::ExpanderGate::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::gate::ExpanderGate::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new expander gate object
 * @param threshold The threshold level in dB
 * @param ratio The expansion ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::gate::ExpanderGate> dibiff::gate::ExpanderGate::create(float& threshold, float& ratio, float& attackTime, float& releaseTime, float& sampleRate) {
    auto instance = std::make_unique<dibiff::gate::ExpanderGate>(threshold, ratio, attackTime, releaseTime, sampleRate);
    instance->initialize();
    return std::move(instance);
}