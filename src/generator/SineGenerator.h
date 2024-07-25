/// SineGenerator.h

#pragma once

#include "generator.h"
#include "../graph/graph.h"

/**
 * @brief Sine Generator
 * @details A sine wave generator object is a simple object that generates a sine
 * wave signal with a certain frequency, sample rate, total number of samples,
 * and block size.
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
class dibiff::generator::SineGenerator : public dibiff::generator::Generator {
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the sine wave source with a certain frequency,
         * sample rate, total number of samples, and block size
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the sine wave
         */
        SineGenerator(float freq, float rate, int samples, int blockSize);
        /**
         * @brief Initialize
         * @details Initializes the sine wave source connection points
         */
        void initialize() override;
        /**
         * @brief Generate a block of samples
         * @details Generates a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the sine wave source
         * @details Resets the current sample index
         */
        void reset() override;
        /**
         * @brief Clear the sine wave source
         * @details Not used.
         */
        void clear() override {}
        /**
         * @brief Get the input connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioInput> getInput(int i = 0) override;
        /**
         * @brief Get the output connection point.
         * @return A shared pointer to the output connection point.
         */
        std::weak_ptr<dibiff::graph::AudioOutput> getOutput() override;
        /**
         * @brief Get the reference connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioReference> getReference() override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * @brief Check if the sine wave source is finished
         * @return True if the sine wave source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new sine wave source object
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the sine wave
         */
        static std::shared_ptr<SineGenerator> create(float freq, float rate, int samples, int blockSize);
        /**
         * Create a new sine wave source object
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param duration The total duration of samples to generate
         * @param blockSize The block size of the sine wave
         */
        static std::shared_ptr<SineGenerator> create(float freq, float rate, std::chrono::duration<float> duration, int blockSize);
    private:
        float frequency;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
};