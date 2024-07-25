/// SquareSource.h

#pragma once

#include "source.h"
#include "../graph/graph.h"
#include "../generator/SquareGenerator.h"
#include "../level/Gain.h"

/**
 * @brief Square Source
 * @details A square wave source object is a simple object that generates a square
 * wave signal with a certain level, frequency, sample rate, total number of samples,
 * block size, and duty cycle. The square wave source object is a composite object that consists
 * of a square wave generator and a gain object.
 */
class dibiff::source::SquareSource : public dibiff::graph::AudioCompositeObject {
    std::shared_ptr<dibiff::generator::SquareGenerator> squareGenerator;
    std::shared_ptr<dibiff::level::Gain> gain;
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
         * @param level The gain of the square wave
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        SquareSource(float level, float freq, float rate, int samples, int blockSize, float dutyCycle = 0.5f);
        /**
         * @brief Initialize
         * @details Initializes the square wave source connection points
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
         * Create a new square wave source object
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param samples The total number of samples to generate
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        static std::shared_ptr<SquareSource> create(float gain, float freq, float rate, int samples, int blockSize, float dutyCycle = 0.5f);
        /**
         * Create a new square wave source object
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param duration The total duration of samples to generate
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        static std::shared_ptr<SquareSource> create(float gain, float freq, float rate, std::chrono::duration<float> duration, int blockSize, float dutyCycle = 0.5f);
        /**
         * Create a new square wave source object
         * @param freq The frequency of the square wave
         * @param rate The sample rate of the square wave
         * @param blockSize The block size of the square wave
         * @param dutyCycle The duty cycle of the square wave (default is 0.5)
         */
        static std::shared_ptr<SquareSource> create(float gain, float freq, float rate, int blockSize, float dutyCycle = 0.5f);
    private:
        float level;
        float frequency;
        float sampleRate;
        int currentSample;
        int totalSamples;
        int blockSize;
        float dutyCycle;
};