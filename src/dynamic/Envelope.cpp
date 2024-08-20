/// Envelope.cpp

#include "Envelope.h"
#include "../inc/Eigen/Dense"

#include <iostream>

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
dibiff::dynamic::Envelope::Envelope(float& attackTime, float& decayTime, float& sustainLevel, float& releaseTime, float& sampleRate)
: dibiff::graph::AudioObject(), 
  attackTime(attackTime), decayTime(decayTime), sustainLevel(sustainLevel), releaseTime(releaseTime), sampleRate(sampleRate) {
    name = "Envelope";
};
/**
 * @brief Initialize
 * @details Initializes the envelope connection points and parameters
 */
void dibiff::dynamic::Envelope::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "EnvelopeInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto mi = std::make_unique<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(this, "EnvelopeMidiInput"));
    _inputs.emplace_back(std::move(mi));
    midiInput = static_cast<dibiff::graph::MidiInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "EnvelopeOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());

    attackIncrement = 1.0f / (attackTime * sampleRate);
    decayIncrement = (1.0f - sustainLevel) / (decayTime * sampleRate);
    releaseIncrement = sustainLevel / (releaseTime * sampleRate);
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::dynamic::Envelope::processAttack(float sample) {
    currentLevel += attackIncrement;
    if (currentLevel >= 1.0f) {
        currentLevel = 1.0f;
        currentStage = Decay;
    }
    return sample * currentLevel;
}
float dibiff::dynamic::Envelope::processDecay(float sample) {
    currentLevel -= decayIncrement;
    if (currentLevel <= sustainLevel) {
        currentLevel = sustainLevel;
        currentStage = Sustain;
    }
    return sample * currentLevel;
}
float dibiff::dynamic::Envelope::processSustain(float sample) {
    return sample * currentLevel;
}
float dibiff::dynamic::Envelope::processRelease(float sample) {
    currentLevel -= releaseIncrement;
    if (currentLevel <= 0.0f) {
        currentLevel = 0.0f;
        currentStage = Idle;
    }
    return sample * currentLevel;
}
float dibiff::dynamic::Envelope::processIdle(float sample) {
    // return sample * currentLevel;
    return 0.0f;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 * @param buffer The input buffer
 * @param blockSize The size of the block
 */
void dibiff::dynamic::Envelope::process() {
    attackIncrement = 1.0f / (attackTime * sampleRate);
    decayIncrement = (1.0f - sustainLevel) / (decayTime * sampleRate);
    releaseIncrement = sustainLevel / (releaseTime * sampleRate);
    if (midiInput->isConnected()) {
        auto& midiData = midiInput->getData();
        int noteOnOff = 0;
        for (const auto& message : midiData) {
            noteOnOff += hasNoteOnNoteOff(message);
        }
        if (noteOnOff > 0) {
            noteOn();
        } else if (noteOnOff < 0) {
            noteOff();
        }
    }
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        const std::vector<float>& inData = input->getData();
        const int inBlockSize = input->getBlockSize();
        std::vector<float> out(inBlockSize, 0.0f);
        output->setData(out, inBlockSize);
    } else if (input->isReady()) {
        const std::vector<float>& data = input->getData();
        const int blockSize = input->getBlockSize();
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = data[i];
        }
        for (int i = 0; i < blockSize; ++i) {
            switch (currentStage) {
                case Attack:
                    y(i) = processAttack(x(i));
                    break;
                case Decay:
                    y(i) = processDecay(x(i));
                    break;
                case Sustain:
                    y(i) = processSustain(x(i));
                    break;
                case Release:
                    y(i) = processRelease(x(i));
                    break;
                case Idle:
                    y(i) = processIdle(x(i));
                    break;
                default:
                    y(i) = processIdle(x(i));
                    break;
            }
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
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new envelope object
 * @param attackTime The attack time in seconds
 * @param decayTime The decay time in seconds
 * @param sustainLevel The sustain level (0 to 1)
 * @param releaseTime The release time in seconds
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::dynamic::Envelope> dibiff::dynamic::Envelope::create(float& attackTime, float& decayTime, float& sustainLevel, float& releaseTime, float& sampleRate) {
    auto instance = std::make_unique<dibiff::dynamic::Envelope>(attackTime, decayTime, sustainLevel, releaseTime, sampleRate);
    instance->initialize();
    return std::move(instance);
}

int dibiff::dynamic::Envelope::hasNoteOnNoteOff(const std::vector<unsigned char> &message) {
    if (message.empty()) return 0;
    if (message.size() < 3) return 0;

    unsigned char status = message[0];
    unsigned char type = status & 0xF0;
    unsigned char velocity = message[2];

    if (type == 0x90 && velocity > 0) { // Note on
        return 1;
    } else if (type == 0x80 || (type == 0x90 && velocity == 0)) { // Note off
        return -2;
    }
    return 0;
}