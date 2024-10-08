/// Envelope.h

#pragma once

#include "dynamic.h"
#include "../graph/graph.h"

/**
 * @brief Envelope
 * @details An envelope is a type of dynamic range compressor that shapes the
 * amplitude of a signal over time. The envelope has attack, decay, sustain, and
 * release times, as well as a sustain level. The envelope is used to shape the
 * amplitude of a signal, such as a musical note.
 * @param attackTime The attack time of the envelope in seconds
 * @param decayTime The decay time of the envelope in seconds
 * @param sustainLevel The sustain level of the envelope (0 to 1)
 * @param releaseTime The release time of the envelope in seconds
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::dynamic::Envelope : public dibiff::graph::AudioObject {
    public:
        dibiff::graph::MidiInput* midiInput;
        dibiff::graph::AudioInput* input;
        dibiff::graph::AudioOutput* output;
        /**
         * Envelope Stages
         */
        enum EnvelopeStage {
            Attack,
            Decay,
            Sustain,
            Release,
            Idle
        };
        EnvelopeStage currentStage = Idle;
        /**
         * @brief Constructor
         * @details Initializes the ADSR envelope with given parameters
         * @param attackTime The attack time in seconds
         * @param decayTime The decay time in seconds
         * @param sustainLevel The sustain level (0 to 1)
         * @param releaseTime The release time in seconds
         * @param sampleRate The sample rate of the input signal
         */
        Envelope(float& attackTime, float& decayTime, float& sustainLevel, float& releaseTime, float& sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the envelope connection points and parameters
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param sample The input sample
         */
        float processAttack(float sample);
        float processDecay(float sample);
        float processSustain(float sample);
        float processRelease(float sample);
        float processIdle(float sample);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         * @param buffer The input buffer
         * @param blockSize The size of the block
         */
        void process() override;
        /**
         * @brief Note on event
         * @details Triggers the envelope to start the attack phase
         */
        void noteOn();
        /**
         * @brief Note off event
         * @details Triggers the envelope to start the release phase
         */
        void noteOff();
        /**
         * @brief Reset the envelope
         * @details Resets the envelope to the idle state
         */
        void reset() override;
        /**
         * @brief Clear the compressor
         * @details Not used
         */
        void clear() override {}
        /**
         * @brief Check if the envelope is finished processing
         * @return True if the envelope is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new envelope object
         * @param attackTime The attack time in seconds
         * @param decayTime The decay time in seconds
         * @param sustainLevel The sustain level (0 to 1)
         * @param releaseTime The release time in seconds
         * @param sampleRate The sample rate of the input signal
         */
        static std::unique_ptr<Envelope> create(float& attackTime, float& decayTime, float& sustainLevel, float& releaseTime, float& sampleRate);
    private:
        float& attackTime;
        float& decayTime;
        float& sustainLevel;
        float& releaseTime;
        float& sampleRate;
        float attackIncrement;
        float decayIncrement;
        float releaseIncrement;
        float currentLevel = 0.0f;
        int hasNoteOnNoteOff(const std::vector<unsigned char>& message);
};