/// Chorus.h

#pragma once

#include "effect.h"
#include "../graph/graph.h"

/**
 * @brief Chorus
 * @details A chorus object is a simple object that adds a chorus effect to
 * the input signal. The chorus object has a certain modulation depth and
 * modulation rate.
 * @param modulationDepth The modulation depth of the chorus in milliseconds
 * @param modulationRate The modulation rate of the chorus in Hz
 * @param sampleRate The sample rate of the input signal
 * @return A chorus object
 */
class dibiff::effect::Chorus : public dibiff::graph::AudioObject {
    public:
        std::shared_ptr<dibiff::graph::AudioInput> input;
        std::shared_ptr<dibiff::graph::AudioOutput> output;
        /**
         * @brief Constructor
         * @details Initializes the chorus object with a certain modulation depth
         * and modulation rate
         * @param modulationDepth The modulation depth of the chorus in milliseconds
         * @param modulationRate The modulation rate of the chorus in Hz
         * @param sampleRate The sample rate of the input signal
         * @param wetLevel The wet level of the chorus
         */
        Chorus(float& modulationDepth, float& modulationRate, float& sampleRate, float& wetLevel);
        /**
         * @brief Initialize
         * @details Initializes the chorus connection points and buffer
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param sample The input sample
         */
        float process(float sample);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the chorus
         * @details Resets the delay buffer
         */
        void reset() override;
        /**
         * @brief Clear the chorus
         * @details Clears the delay buffer
         */
        void clear() override;
        /**
         * @brief Check if the chorus is finished processing
         * @return True if the chorus is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new chorus object
         * @param modulationDepth The modulation depth of the chorus in milliseconds
         * @param modulationRate The modulation rate of the chorus in Hz
         * @param sampleRate The sample rate of the input signal
         * @param wetLevel The wet level of the chorus
         */
        static std::shared_ptr<Chorus> create(float& modulationDepth, float& modulationRate, float& sampleRate, float& wetLevel);
    private:
        float& modulationDepth;
        float& modulationRate;
        float& sampleRate;
        float& wetLevel;
        std::vector<float> buffer;
        int bufferIndex = 0;
        int maxDelaySamples;
        float phase = 0.0f;
};