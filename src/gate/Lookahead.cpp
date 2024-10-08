/// LookaheadGate.cpp

#include "LookaheadGate.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the lookahead gate object with a certain threshold,
 * attack time, release time, lookahead time, and sample rate
 * @param threshold The threshold level in dB
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param lookaheadTime The lookahead time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::gate::LookaheadGate::LookaheadGate(float& threshold, float& attackTime, float& releaseTime, float& lookaheadTime, float& sampleRate)
: dibiff::graph::AudioObject(), 
  _threshold(threshold), _attackTime(attackTime), _releaseTime(releaseTime), _lookaheadTime(lookaheadTime), _sampleRate(sampleRate) {
    name = "LookaheadGate";
};
/**
 * @brief Initialize
 * @details Initializes the lookahead gate connection points and envelope
 */
void dibiff::gate::LookaheadGate::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "LookaheadGateInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "LookaheadGateOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
    _attackCoefficient = std::exp(-1.0f / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = std::exp(-1.0f / (_releaseTime * _sampleRate / 1000.0f));
    _thresholdLevel = std::pow(10.0f, _threshold / 20.0f);
    _lookaheadSamples = static_cast<int>(_lookaheadTime * _sampleRate / 1000.0f);
    _delayBuffer.resize(_lookaheadSamples, 0.0f);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 */
float dibiff::gate::LookaheadGate::process(float sample) {
    float sidechainInput = _delayBuffer[_bufferIndex];
    float sidechainLevel = std::fabs(sidechainInput);
    if (sidechainLevel > _thresholdLevel) {
        _envelope = _attackCoefficient * (_envelope - sidechainLevel) + sidechainLevel;
    } else {
        _envelope = _releaseCoefficient * _envelope;
    }
    float out;
    if (sidechainLevel < _thresholdLevel) {
        float gainReduction = _envelope < _thresholdLevel ? 0.0f : 1.0f;
        out = sample * gainReduction;
    } else {
        out = sample;
    }
    _delayBuffer[_bufferIndex] = sample;
    _bufferIndex = (_bufferIndex + 1) % _lookaheadSamples;
    return out;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param buffer The input buffer
 * @param blockSize The size of the block
 */
void dibiff::gate::LookaheadGate::process() {
    _attackCoefficient = std::exp(-1.0f / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = std::exp(-1.0f / (_releaseTime * _sampleRate / 1000.0f));
    _thresholdLevel = std::pow(10.0f, _threshold / 20.0f);
    /// TODO: Handle changes to buffer stuff
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
 * @brief Reset the lookahead gate
 * @details Resets the envelope and delay buffer
 */
void dibiff::gate::LookaheadGate::reset() {
    _envelope = 0.0f;
    std::fill(_delayBuffer.begin(), _delayBuffer.end(), 0.0f);
}
/**
 * @brief Clear the lookahead gate
 * @details Clears the delay buffer
 */
void dibiff::gate::LookaheadGate::clear() {
    reset();
}
/**
 * @brief Check if the lookahead gate is finished processing
 * @return True if the lookahead gate is finished processing, false otherwise
 */
bool dibiff::gate::LookaheadGate::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::gate::LookaheadGate::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new lookahead gate object
 * @param threshold The threshold level in dB
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param lookaheadTime The lookahead time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::gate::LookaheadGate> dibiff::gate::LookaheadGate::create(float& threshold, float& attackTime, float& releaseTime, float& lookaheadTime, float& sampleRate) {
    auto instance = std::make_unique<dibiff::gate::LookaheadGate>(threshold, attackTime, releaseTime, lookaheadTime, sampleRate);
    instance->initialize();
    return std::move(instance);
}