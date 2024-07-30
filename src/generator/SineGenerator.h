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
    std::shared_ptr<dibiff::graph::MidiInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the sine wave source with a sample rate, 
         * total number of samples, and block size
         * @param blockSize The block size of the sine wave
         * @param sampleRate The sample rate of the sine wave
         * @param frequency The frequency of the sine wave, used if the MIDI input is not connected
         * @param totalSamples The total number of samples to generate
         */
        SineGenerator(int blockSize, int sampleRate, float frequency = 1000.0f, int totalSamples = -1);
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
         * @brief Check if the sine wave source is finished
         * @return True if the sine wave source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new sine wave source object
         * @param rate The sample rate of the sine wave
         * @param blockSize The block size of the sine wave
         * @param frequency The frequency of the sine wave, used if the MIDI input is not connected
         * @param totalSamples The total number of samples to generate
         */
        static std::shared_ptr<SineGenerator> create(int blockSize, float sampleRate, float frequency = 1000.0f, int totalSamples = -1);
        /**
         * Create a new sine wave source object
         * @param rate The sample rate of the sine wave
         * @param blockSize The block size of the sine wave
         * @param frequency The frequency of the sine wave, used if the MIDI input is not connected
         * @param totalSamples The total number of samples to generate
         */
        static std::shared_ptr<SineGenerator> create(int blockSize, float sampleRate, float frequency, std::chrono::duration<int> duration);
    private:
        int blockSize;
        int sampleRate;
        float frequency;
        int totalSamples;
        int currentSample;
        float phase;
};