/// TriangleSource.h

#pragma once

#include "source.h"
#include "../graph/graph.h"
#include "../generator/TriangleGenerator.h"
#include "../level/Gain.h"   

/**
 * @brief Triangle Source
 * @details A triangle wave source object is a simple object that generates a triangle
 * wave signal with a certain level, frequency, sample rate, total number of samples,
 * and block size. The triangle wave source object is a composite object that consists
 * of a triangle wave generator and a gain object.
 */
class dibiff::source::TriangleSource : public dibiff::graph::AudioCompositeObject {
    std::shared_ptr<dibiff::generator::TriangleGenerator> triangleGenerator;
    std::shared_ptr<dibiff::level::Gain> gain;
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
         * @param level The gain of the triangle wave
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the triangle wave
         */
        TriangleSource(float level, float freq, float rate, int samples, int blockSize);
        /**
         * @brief Initialize
         * @details Initializes the triangle wave source connection points
         */
        void initialize() override;
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
         * Create a new triangle wave source object
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the triangle wave
         */
        static std::shared_ptr<TriangleSource> create(float gain, float freq, float rate, int samples, int blockSize);
        /**
         * Create a new triangle wave source object
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param duration The total duration of samples to generate
         * @param blockSize The block size of the triangle wave
         */
        static std::shared_ptr<TriangleSource> create(float gain, float freq, float rate, std::chrono::duration<float> duration, int blockSize);
        /**
         * Create a new triangle wave source object
         * @param freq The frequency of the triangle wave
         * @param rate The sample rate of the triangle wave
         * @param blockSize The block size of the triangle wave
         */
        static std::shared_ptr<TriangleSource> create(float gain, float freq, float rate, int blockSize);
    private:
        float level;
        float frequency;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
};