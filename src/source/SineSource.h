/// SineSource.h

#pragma once

#include "source.h"
#include "../graph/graph.h"
#include "../generator/SineGenerator.h"
#include "../level/Gain.h"

/**
 * @brief Sine Source
 * @details A sine wave source object is a simple object that generates a sine
 * wave signal with a certain level, frequency, sample rate, total number of samples,
 * and block size. The sine wave source object is a composite object that consists
 * of a sine wave generator and a gain object.
 */
class dibiff::source::SineSource : public dibiff::graph::AudioCompositeObject {
    std::shared_ptr<dibiff::generator::SineGenerator> sineGenerator;
    std::shared_ptr<dibiff::level::Gain> gain;
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
         * @param level The gain of the sine wave
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the sine wave
         */
        SineSource(float level, float freq, float rate, int samples, int blockSize);
        /**
         * @brief Initialize
         * @details Initializes the sine wave source connection points
         */
        void initialize() override;
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
         * Create a new sine wave source object
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the sine wave
         */
        static std::shared_ptr<SineSource> create(float gain, float freq, float rate, int samples, int blockSize);
        /**
         * Create a new sine wave source object
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param duration The total duration of samples to generate
         * @param blockSize The block size of the sine wave
         */
        static std::shared_ptr<SineSource> create(float gain, float freq, float rate, std::chrono::duration<float> duration, int blockSize);
        /**
         * Create a new sine wave source object
         * @param freq The frequency of the sine wave
         * @param rate The sample rate of the sine wave
         * @param blockSize The block size of the sine wave
         */
        static std::shared_ptr<SineSource> create(float gain, float freq, float rate, int blockSize);
    private:
        float level;
        float frequency;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
};