/// Tremolo.h

#pragma once

#include "effect.h"
#include "../graph/graph.h"

/**
 * @brief Tremolo
 * @details A tremolo object is a simple object that adds a tremolo effect to
 * the input signal. The tremolo object has a certain modulation depth and
 * modulation rate.
 * @param modulationDepth The modulation depth of the tremolo in milliseconds
 * @param modulationRate The modulation rate of the tremolo in Hz
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::effect::Tremolo : public dibiff::graph::AudioObject {
    public:
        dibiff::graph::AudioInput* input;
        dibiff::graph::AudioOutput* output;
        /**
         * @brief Constructor
         * @details Initializes the tremolo object with a certain modulation depth
         * and modulation rate
         * @param modulationDepth The modulation depth of the tremolo (0 to 1)
         * @param modulationRate The modulation rate of the tremolo in Hz
         * @param sampleRate The sample rate of the input signal
         */
        Tremolo(float& modulationDepth, float& modulationRate, float& sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the tremolo connection points
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
         */
        void process() override;
        /**
         * @brief Reset the tremolo
         * @details Resets the phase of the LFO
         */
        void reset() override;
        /**
         * @brief Clear the vibrato
         * @details Not used.
         */
        void clear() override {}
        /**
         * @brief Check if the tremolo is finished processing
         * @return True if the tremolo is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new tremolo object
         * @param modulationDepth The modulation depth of the tremolo in milliseconds
         * @param modulationRate The modulation rate of the tremolo in Hz
         * @param sampleRate The sample rate of the input signal
         */
        static std::unique_ptr<Tremolo> create(float& modulationDepth, float& modulationRate, float& sampleRate);
    private:
        float& modulationDepth;
        float& modulationRate;
        float& sampleRate;
        float phase = 0.0f;
};