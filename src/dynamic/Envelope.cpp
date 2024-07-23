/// Envelope.cpp

#include "Envelope.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::dynamic::Envelope::getName() const { return "Envelope"; }
enum EnvelopeStage {
    Attack,
    Decay,
    Sustain,
    Release,
    Idle
};
/**
 * @brief Constructor
 * @details Initializes the ADSR envelope with given parameters
 * @param attackTime The attack time in seconds
 * @param decayTime The decay time in seconds
 * @param sustainLevel The sustain level (0 to 1)
 * @param releaseTime The release time in seconds
 * @param sampleRate The sample rate of the input signal
 */
dibiff::dynamic::Envelope::Envelope(float attackTime, float decayTime, float sustainLevel, float releaseTime, float sampleRate)
: dibiff::graph::AudioObject(), 
    attackTime(attackTime), decayTime(decayTime), sustainLevel(sustainLevel), releaseTime(releaseTime), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the envelope connection points and parameters
 */
void dibiff::dynamic::Envelope::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "EnvelopeInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "EnvelopeOutput"));
    attackIncrement = 1.0f / (attackTime * sampleRate);
    decayIncrement = (1.0f - sustainLevel) / (decayTime * sampleRate);
    releaseIncrement = sustainLevel / (releaseTime * sampleRate);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::dynamic::Envelope::process(float sample) {
    switch (currentStage) {
        case Attack:
            currentLevel += attackIncrement;
            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
                currentStage = Decay;
            }
            break;
        case Decay:
            currentLevel -= decayIncrement;
            if (currentLevel <= sustainLevel) {
                currentLevel = sustainLevel;
                currentStage = Sustain;
            }
            break;
        case Sustain:
            // Sustain level is maintained until note off
            break;
        case Release:
            currentLevel -= releaseIncrement;
            if (currentLevel <= 0.0f) {
                currentLevel = 0.0f;
                currentStage = Idle;
            }
            break;
        case Idle:
            // Do nothing
            break;
    }
    return sample * currentLevel;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param buffer The input buffer
 * @param blockSize The size of the block
 */
void dibiff::dynamic::Envelope::process() {
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
 * @brief Note on event
 * @details Triggers the envelope to start the attack phase
 */
void dibiff::dynamic::Envelope::noteOn() {
    currentStage = Attack;
    currentLevel = 0.0f;
}
/**
 * @brief Note off event
 * @details Triggers the envelope to start the release phase
 */
void dibiff::dynamic::Envelope::noteOff() {
    currentStage = Release;
}
/**
 * @brief Reset the envelope
 * @details Resets the envelope to the idle state
 */
void dibiff::dynamic::Envelope::reset() {
    currentStage = Idle;
    currentLevel = 0.0f;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::dynamic::Envelope::getInput() { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::dynamic::Envelope::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::dynamic::Envelope::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the envelope is finished processing
 * @return True if the envelope is finished processing, false otherwise
 */
bool dibiff::dynamic::Envelope::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::dynamic::Envelope::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new envelope object
 * @param attackTime The attack time in seconds
 * @param decayTime The decay time in seconds
 * @param sustainLevel The sustain level (0 to 1)
 * @param releaseTime The release time in seconds
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::dynamic::Envelope> dibiff::dynamic::Envelope::create(float attackTime, float decayTime, float sustainLevel, float releaseTime, float sampleRate) {
    auto instance = std::make_shared<dibiff::dynamic::Envelope>(attackTime, decayTime, sustainLevel, releaseTime, sampleRate);
    instance->initialize();
    return instance;
}