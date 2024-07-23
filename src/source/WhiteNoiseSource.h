/// WhiteNoiseSource.h

#pragma once

#include "source.h"
#include "../graph/graph.h"
#include "../generator/WhiteNoiseGenerator.h"
#include "../level/Gain.h"

/**
 * @brief White Noise Source
 * @details A white noise source object is a simple object that generates white
 * noise with a certain level, sample rate, total number of samples, and block size.
 * The white noise source object is a composite object that consists of a white noise
 * generator and a gain object.
 */
class dibiff::source::WhiteNoiseSource : public dibiff::graph::AudioCompositeObject {
    std::shared_ptr<dibiff::generator::WhiteNoiseGenerator> whiteNoiseGenerator;
    std::shared_ptr<dibiff::level::Gain> gain;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the white noise source with a certain level, sample rate,
         * total number of samples, and block size
         * @param level The gain of the white noise
         * @param rate The sample rate of the white noise
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the white noise
         */
        WhiteNoiseSource(float level, float rate, int samples, int blockSize);
        /**
         * @brief Initialize
         * @details Initializes the white noise source connection points
         */
        void initialize() override;
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
         * Create a new white noise source object
         * @param rate The sample rate of the white noise
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the white noise
         */
        static std::shared_ptr<WhiteNoiseSource> create(float gain, float rate, int samples, int blockSize);
        /**
         * Create a new white noise source object
         * @param rate The sample rate of the white noise
         * @param duration The total duration of samples to generate
         * @param blockSize The block size of the white noise
         */
        static std::shared_ptr<WhiteNoiseSource> create(float gain, float rate, std::chrono::duration<float> duration, int blockSize);
        /**
         * Create a new white noise source object
         * @param rate The sample rate of the white noise
         * @param blockSize The block size of the white noise
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the white noise
         */
        static std::shared_ptr<WhiteNoiseSource> create(float gain, float rate, int blockSize);
    private:
        float level;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
};