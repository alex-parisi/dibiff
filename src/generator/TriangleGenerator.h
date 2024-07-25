/// TriangleGenerator.h

#pragma once

#include "generator.h"
#include "../graph/graph.h"

/**
 * @brief Triangle Generator
 * @details A triangle wave generator object is a simple object that generates a triangle
 * wave signal with a certain frequency, sample rate, total number of samples,
 * and block size.
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the triangle wave
 */
class dibiff::generator::TriangleGenerator : public dibiff::generator::Generator {
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the triangle wave source with a certain frequency,
         * sample rate, total number of samples, and block size
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the triangle wave
         */
        TriangleGenerator(float freq, float rate, int samples, int blockSize);
        /**
         * @brief Initialize
         * @details Initializes the triangle wave source connection points
         */
        void initialize() override;
        /**
         * @brief Generate a block of samples
         * @details Generates a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the triangle wave source
         * @details Resets the current sample index
         */
        void reset() override;
        /**
         * @brief Clear the triangle wave source
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
         * @brief Check if the triangle wave source is finished
         * @return True if the triangle wave source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new triangle wave source object
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the triangle wave
         */
        static std::shared_ptr<TriangleGenerator> create(float freq, float rate, int samples, int blockSize);
        /**
         * Create a new triangle wave source object
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param duration The total number of samples to generate
         * @param blockSize The block size of the triangle wave
         */
        static std::shared_ptr<TriangleGenerator> create(float freq, float rate, std::chrono::duration<float> duration, int blockSize);
    private:
        float frequency;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
};