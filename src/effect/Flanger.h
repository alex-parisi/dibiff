/// Flanger.h

#pragma once

#include "effect.h"
#include "../graph/graph.h"

/**
 * @brief Flanger
 * @details A flanger object is a simple object that adds a flanger effect to
 * the input signal. The flanger object has a certain modulation depth and
 * modulation rate.
 * @param modulationDepth The modulation depth of the flanger in milliseconds
 * @param modulationRate The modulation rate of the flanger in Hz
 * @param sampleRate The sample rate of the input signal
 * @return A flanger object
 */
class dibiff::effect::Flanger : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the flanger object with a certain modulation depth
         * and modulation rate
         * @param modulationDepth The modulation depth of the flanger in milliseconds
         * @param modulationRate The modulation rate of the flanger in Hz
         * @param sampleRate The sample rate of the input signal
         * @param feedback The feedback amount of the flanger effect
         * @param wetLevel The wet level of the flanger
         */
        Flanger(float& modulationDepth, float& modulationRate, float& sampleRate, float& feedback, float& wetLevel);
        /**
         * @brief Initialize
         * @details Initializes the flanger connection points and buffer
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
         * @brief Reset the flanger
         * @details Resets the delay buffer
         */
        void reset() override;
        /**
         * @brief Clear the flanger
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
         * @brief Check if the flanger is finished processing
         * @return True if the flanger is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new flanger object
         * @param modulationDepth The modulation depth of the flanger in milliseconds
         * @param modulationRate The modulation rate of the flanger in Hz
         * @param sampleRate The sample rate of the input signal
         * @param feedback The feedback amount of the flanger effect
         * @param wetLevel The wet level of the flanger
         */
        static std::shared_ptr<Flanger> create(float& modulationDepth, float& modulationRate, float& sampleRate, float& feedback, float& wetLevel);
    private:
        float& modulationDepth;
        float& modulationRate;
        float& sampleRate;
        float& feedback;
        float& wetLevel;
        std::vector<float> buffer;
        int bufferIndex = 0;
        int maxDelaySamples;
        float phase = 0.0f;
};