/// LookaheadGate.h

#pragma once

#include "gate.h"
#include "../graph/graph.h"

/**
 * @brief Lookahead Gate
 * @details A lookahead gate object is a simple object that introduces a small 
 * delay to anticipate when to open the gate, providing smoother gating and 
 * reducing transient artifacts.
 * @param threshold The threshold level of the lookahead gate in dB
 * @param attackTime The attack time of the lookahead gate in milliseconds
 * @param releaseTime The release time of the lookahead gate in milliseconds
 * @param lookaheadTime The lookahead time of the lookahead gate in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::gate::LookaheadGate : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
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
        LookaheadGate(float& threshold, float& attackTime, float& releaseTime, float& lookaheadTime, float& sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the lookahead gate connection points and envelope
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param input The input sample
         */
        float process(float sample);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         * @param buffer The input buffer
         * @param blockSize The size of the block
         */
        void process() override;
        /**
         * @brief Reset the lookahead gate
         * @details Resets the envelope and delay buffer
         */
        void reset() override;
        /**
         * @brief Clear the lookahead gate
         * @details Clears the delay buffer
         */
        void clear() override;
        /**
         * @brief Get the input connection point.
         * @return A shared pointer to the input connection point.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) override;
        /**
         * @brief Get the output connection point.
         * @return A shared pointer to the output connection point.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) override;
        /**
         * @brief Get the reference connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getReference() override;
        /**
         * @brief Check if the lookahead gate is finished processing
         * @return True if the lookahead gate is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new lookahead gate object
         * @param threshold The threshold level in dB
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param lookaheadTime The lookahead time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        static std::shared_ptr<LookaheadGate> create(float& threshold, float& attackTime, float& releaseTime, float& lookaheadTime, float& sampleRate);
    private:
        float& _threshold;
        float& _attackTime;
        float& _releaseTime;
        float& _lookaheadTime;
        float& _sampleRate;
        float _envelope = 0.0f;
        float _attackCoefficient;
        float _releaseCoefficient;
        float _thresholdLevel;
        int _lookaheadSamples;
        int _bufferIndex = 0;
        std::vector<float> _delayBuffer;
};