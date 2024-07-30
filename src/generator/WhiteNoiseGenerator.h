/// WhiteNoiseGenerator.h

#pragma once

#include "generator.h"
#include "../graph/graph.h"
#include <random>

/**
 * @brief White Noise Generator
 * @details A white noise generator object is a simple object that generates white
 * noise with a certain sample rate, total number of samples, and block size.
 */
class dibiff::generator::WhiteNoiseGenerator : public dibiff::generator::Generator {
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the white noise source with a certain frequency,
         * sample rate, total number of samples, and block size
         * @param blockSize The block size of the white noise
         * @param sampleRate The sample rate of the white noise
         * @param totalSamples The total number of samples to generate
         */
        WhiteNoiseGenerator(int blockSize, int sampleRate, int totalSamples = -1);
        /**
         * @brief Initialize
         * @details Initializes the white noise source connection points
         */
        void initialize() override;
        /**
         * @brief Generate a block of samples
         * @details Generates a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the white noise source
         * @details Resets the current sample index
         */
        void reset() override;
        /**
         * @brief Clear the white noise source
         * @details Not used.
         */
        void clear() override {}
        /**
         * @brief Get the input connection point.
         * @return Not used.
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
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * @brief Check if the white noise source is finished
         * @return True if the white noise source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new white noise source object
         * @param blockSize The block size of the white noise
         * @param sampleRate The sample rate of the white noise
         * @param totalSamples The total number of samples to generate
         */
        static std::shared_ptr<WhiteNoiseGenerator> create(int blockSize, int sampleRate, int totalSamples = -1);
        /**
         * Create a new white noise source object
         * @param blockSize The block size of the white noise
         * @param sampleRate The sample rate of the white noise
         * @param duration The total duration of samples to generate
         */
        static std::shared_ptr<WhiteNoiseGenerator> create(int blockSize, int sampleRate, std::chrono::duration<int> duration);
    private:
        int blockSize;
        int sampleRate;
        int totalSamples;
        int currentSample;
        std::random_device rd;
        std::default_random_engine engine;
        std::uniform_real_distribution<float> distribution;
};