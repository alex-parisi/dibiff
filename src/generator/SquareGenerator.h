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
    std::shared_ptr<dibiff::graph::MidiInput> input;
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
         * @param blockSize The block size of the square wave
         * @param sampleRate The sample rate of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         * @param frequency The frequency of the square wave, used if the MIDI input is not connected
         * @param totalSamples The total number of samples to generate
         */
        SquareGenerator(int blockSize, int sampleRate, float dutyCycle = 0.5f, float frequency = 1000.0f, int totalSamples = -1);
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
         * @brief Check if the square wave source is finished
         * @return True if the square wave source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new square wave source object
         * @param blockSize The block size of the square wave
         * @param sampleRate The sample rate of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         * @param frequency The frequency of the square wave, used if the MIDI input is not connected
         * @param totalSamples The total number of samples to generate
         */
        static std::shared_ptr<SquareGenerator> create(int blockSize, int sampleRate, float dutyCycle = 0.5f, float frequency = 1000.0f, int totalSamples = -1);
        /**
         * Create a new square wave source object
         * @param blockSize The block size of the square wave
         * @param sampleRate The sample rate of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         * @param frequency The frequency of the square wave, used if the MIDI input is not connected
         * @param duration The length of time to generate samples
         */
        static std::shared_ptr<SquareGenerator> create(int blockSize, int sampleRate, float dutyCycle, float frequency, std::chrono::duration<int> duration);
    private:
        int blockSize;
        int sampleRate;
        float dutyCycle;
        float frequency;
        int totalSamples;
        int currentSample;
};