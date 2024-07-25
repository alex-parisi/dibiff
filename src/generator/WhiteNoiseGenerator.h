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
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the white noise source with a certain frequency,
         * sample rate, total number of samples, and block size
         * @param freq The frequency of the white noise
         * @param rate The sample rate of the white noise
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the white noise
         */
        WhiteNoiseGenerator(float rate, int samples, int blockSize);
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
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput() override;
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
         * @param freq The frequency of the white noise
         * @param rate The sample rate of the white noise
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the white noise
         */
        static std::shared_ptr<WhiteNoiseGenerator> create(float rate, int samples, int blockSize);
        /**
         * Create a new white noise source object
         * @param freq The frequency of the white noise
         * @param rate The sample rate of the white noise
         * @param duration The total duration of samples to generate
         * @param blockSize The block size of the white noise
         */
        static std::shared_ptr<WhiteNoiseGenerator> create(float rate, std::chrono::duration<float> duration, int blockSize);
    private:
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
        std::random_device rd;
        std::default_random_engine engine;
        std::uniform_real_distribution<float> distribution;
};