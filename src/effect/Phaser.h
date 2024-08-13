/// Phaser.h

#pragma once

#include "effect.h"
#include "../graph/graph.h"
#include "../filter/AllPassFilter.h"

/**
 * @brief Phaser
 * @details A phaser object is a simple object that adds a phaser effect to
 * the input signal. The phaser object has a certain modulation depth and
 * modulation rate.
 * @param modulationDepth The modulation depth of the phaser in milliseconds
 * @param modulationRate The modulation rate of the phaser in Hz
 * @param sampleRate The sample rate of the input signal
 * @return A phaser object
 */
class dibiff::effect::Phaser : public dibiff::graph::AudioObject {
    public:
        std::shared_ptr<dibiff::graph::AudioInput> input;
        std::shared_ptr<dibiff::graph::AudioOutput> output;
        /**
         * @brief Constructor
         * @details Initializes the phaser object with a certain modulation depth
         * and modulation rate
         * @param modulationDepth The modulation depth of the phaser in Hz
         * @param modulationRate The modulation rate of the phaser in Hz
         * @param sampleRate The sample rate of the input signal
         * @param baseCutoff The base cutoff frequency of the phaser, default value is 1000.0 Hz
         * @param mix The mix of the phaser effect, default value is 0.5
         * @param numStages The number of all-pass filter in the phaser, default value is 4
         * @param wetLevel The wet level of the phaser
         */
        Phaser(float& modulationDepth, float& modulationRate, float& sampleRate, float& baseCutoff, float& mix, int& numStages);
        /**
         * @brief Initialize
         * @details Initializes the phaser connection points and all-pass filters
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
         * @brief Reset the phaser
         * @details Resets the all-pass filter
         */
        void reset() override;
        /**
         * @brief Clear the phaser
         * @details Clears the all-pass filter
         */
        void clear() override;
        /**
         * @brief Check if the phaser is finished processing
         * @return True if the phaser is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new phaser object
         * @param modulationDepth The modulation depth of the phaser in milliseconds
         * @param modulationRate The modulation rate of the phaser in Hz
         * @param sampleRate The sample rate of the input signal
         * @param baseCutoff The base cutoff frequency of the phaser, default value is 1000.0 Hz
         * @param mix The mix of the phaser effect, default value is 0.5
         * @param numStages The number of all-pass filter in the phaser, default value is 4
         */
        static std::shared_ptr<Phaser> create(float& modulationDepth, float& modulationRate, float& sampleRate, float& baseCutoff, float& mix, int& numStages);
    private:
        float& modulationDepth;
        float& modulationRate;
        float& sampleRate;
        float phase = 0.0f;
        float& baseCutoff;
        float& mix;
        int& numStages;
        std::vector<std::shared_ptr<dibiff::filter::AllPassFilter>> allPassFilters;
};