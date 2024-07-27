/// Ducker.cpp

#include "Ducker.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::gate::Ducker::getName() const { return "Ducker"; }
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
dibiff::gate::Ducker::Ducker(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate)
: dibiff::graph::AudioObject(), 
    threshold(threshold), ratio(ratio), attackTime(attackTime), releaseTime(releaseTime), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the ducker connection points and envelope
 */
void dibiff::gate::Ducker::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "DuckerInput"));
    reference = std::make_shared<dibiff::graph::AudioReference>(dibiff::graph::AudioReference(shared_from_this(), "DuckerReference"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "DuckerOutput"));
    attackCoefficient = std::exp(-1.0f / (attackTime * sampleRate / 1000.0f));
    releaseCoefficient = std::exp(-1.0f / (releaseTime * sampleRate / 1000.0f));
    thresholdLevel = std::pow(10.0f, threshold / 20.0f); // Convert dB to linear
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The main input sample
 * @param sidechain The sidechain input sample
 */
float dibiff::gate::Ducker::process(float sample, float reference) {
    float sidechainLevel = std::fabs(reference);
    if (sidechainLevel > thresholdLevel) {
        envelope = attackCoefficient * (envelope - sidechainLevel) + sidechainLevel;
    } else {
        envelope = releaseCoefficient * envelope;
    }
    if (sidechainLevel > thresholdLevel) {
        float gainReduction = std::pow(envelope / thresholdLevel, 1.0f - ratio);
        return sample * gainReduction;
    }
    return sample;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::gate::Ducker::process() {
    if (input->isReady() && reference->isReady()) {
        std::vector<float> inData = *input->getData();
        std::vector<float> refData = *reference->getData();
        int inBlockSize = input->getBlockSize();
        int refBlockSize = reference->getBlockSize();
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
    envelope = 0.0f;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::gate::Ducker::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::gate::Ducker::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::gate::Ducker::getReference() { return reference; };
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
    // Check if input is connected
    return input->isConnected() && input->isReady() && reference->isConnected() && reference->isReady() && !processed;
}
/**
 * Create a new ducker object
 * @param threshold The threshold level in dB
 * @param ratio The attenuation ratio (greater than 1)
 * @param attackTime The attack time in milliseconds
 * @param releaseTime The release time in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::gate::Ducker> dibiff::gate::Ducker::create(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate) {
    auto instance = std::make_shared<dibiff::gate::Ducker>(threshold, ratio, attackTime, releaseTime, sampleRate);
    instance->initialize();
    return instance;
}