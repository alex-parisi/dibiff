/// NoiseGate.cpp

#include "NoiseGate.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the noise gate object with a certain threshold,
 * attack time, release time, and sample rate
 * @param threshold The threshold level in dB
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::gate::NoiseGate::NoiseGate(float& threshold, float& attackTime, float& releaseTime, float& sampleRate)
: dibiff::graph::AudioObject(), 
  _threshold(threshold), _attackTime(attackTime), _releaseTime(releaseTime), _sampleRate(sampleRate) {
    name = "NoiseGate";
};
/**
 * @brief Initialize
 * @details Initializes the noise gate connection points and envelope
 */
void dibiff::gate::NoiseGate::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "NoiseGateInput"));
    _inputs.push_back(input);
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "NoiseGateOutput"));
    _outputs.push_back(output);
    _attackCoefficient = std::exp(-1.0 / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = std::exp(-1.0 / (_releaseTime * _sampleRate / 1000.0f));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 */
float dibiff::gate::NoiseGate::process(float sample) {
    float inputLevel = std::fabs(sample);
    if (inputLevel > _thresholdLevel) {
        _envelope = _attackCoefficient * (_envelope - inputLevel) + inputLevel;
    } else {
        _envelope = _releaseCoefficient * _envelope;
    }
    if (_envelope < _thresholdLevel) {
        return 0.0f;
    }
    return sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::gate::NoiseGate::process() {
    _attackCoefficient = std::exp(-1.0 / (_attackTime * _sampleRate / 1000.0f));
    _releaseCoefficient = std::exp(-1.0 / (_releaseTime * _sampleRate / 1000.0f));
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
 * @brief Reset the noise gate
 * @details Resets the envelope to zero
 */
void dibiff::gate::NoiseGate::reset() {
    _envelope = 0.0f;
}
/**
 * @brief Check if the noise gate is finished processing
 * @return True if the noise gate is finished processing, false otherwise
 */
bool dibiff::gate::NoiseGate::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::gate::NoiseGate::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new noise gate object
 * @param threshold The threshold level in dB
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::gate::NoiseGate> dibiff::gate::NoiseGate::create(float& threshold, float& attackTime, float& releaseTime, float& sampleRate) {
    auto instance = std::make_shared<dibiff::gate::NoiseGate>(threshold, attackTime, releaseTime, sampleRate);
    instance->initialize();
    return instance;
}