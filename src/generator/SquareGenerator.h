/// SquareGenerator.h

#pragma once

#include "generator.h"
#include "../graph/graph.h"

/**
 * @brief Square Generator
 * @details A square wave generator object is a simple object that generates a square
 * wave signal with a certain frequency, sample rate, total number of samples,
 * block size, and duty cycle.
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
class dibiff::generator::SquareGenerator : public dibiff::generator::Generator {
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the square wave source with a certain frequency,
         * sample rate, total number of samples, block size, and duty cycle
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        SquareGenerator(float freq, float rate, int samples, int blockSize, float dutyCycle = 0.5f);
        /**
         * @brief Initialize
         * @details Initializes the square wave source connection points
         */
        void initialize() override;
        /**
         * @brief Generate a block of samples
         * @details Generates a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the square wave source
         * @details Resets the current sample index
         */
        void reset() override;
        /**
         * @brief Clear the square wave source
         * @details Not used.
         */
        void clear() override {}
        /**
         * @brief Get the input connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioInput> getInput() override;
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
         * @brief Check if the square wave source is finished
         * @return True if the square wave source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new square wave source object
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        static std::shared_ptr<SquareGenerator> create(float freq, float rate, int samples, int blockSize, float dutyCycle = 0.5f);
        /**
         * Create a new square wave source object
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param duration The total number of samples to generate
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        static std::shared_ptr<SquareGenerator> create(float freq, float rate, std::chrono::duration<float> duration, int blockSize, float dutyCycle = 0.5f);
    private:
        float frequency;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
        float dutyCycle;
};