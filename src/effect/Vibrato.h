/// Vibrato.h

#pragma once

#include "effect.h"
#include "../graph/graph.h"

/**
 * @brief Vibrato
 * @details A vibrato object is a simple
 * object that adds a vibrato effect to the input signal. The vibrato object
 * has a certain modulation depth and modulation rate.
 * @param modulationDepth The modulation depth of the vibrato in milliseconds
 * @param modulationRate The modulation rate of the vibrato in Hz
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::effect::Vibrato : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the vibrato object with a certain modulation depth
         * and modulation rate
         * @param modulationDepth The modulation depth of the vibrato in milliseconds
         * @param modulationRate The modulation rate of the vibrato in Hz
         * @param sampleRate The sample rate of the input signal
         */
        Vibrato(float modulationDepth, float modulationRate, float sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the vibrato connection points and buffer
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
         * @brief Reset the vibrato
         * @details Resets the delay buffer
         */
        void reset() override;
        /**
         * @brief Clear the vibrato
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
         * @brief Check if the vibrato is finished processing
         * @return True if the vibrato is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new vibrato object
         * @param modulationDepth The modulation depth of the vibrato in milliseconds
         * @param modulationRate The modulation rate of the vibrato in Hz
         * @param sampleRate The sample rate of the input signal
         */
        static std::shared_ptr<Vibrato> create(float modulationDepth, float modulationRate, float sampleRate);
    private:
        float modulationDepth;
        float modulationRate;
        float sampleRate;
        float phase = 0.0f;
        int maxDelaySamples;
        int bufferIndex = 0;
        std::vector<float> buffer;
};