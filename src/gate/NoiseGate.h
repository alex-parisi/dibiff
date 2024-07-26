/// NoiseGate.h

#pragma once

#include "gate.h"
#include "../graph/graph.h"

/**
 * @brief Noise Gate
 * @details A noise gate object is a simple object that gates the input signal
 * based on a certain threshold level. The noise gate object has attack and
 * release times.
 * @param threshold The threshold level of the noise gate in dB
 * @param attackTime The attack time of the noise gate in milliseconds
 * @param releaseTime The release time of the noise gate in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::gate::NoiseGate : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the noise gate object with a certain threshold,
         * attack time, release time, and sample rate
         * @param threshold The threshold level in dB
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        NoiseGate(float threshold, float attackTime, float releaseTime, float sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the noise gate connection points and envelope
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param input The input sample
         */
        float process(float sample);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the noise gate
         * @details Resets the envelope to zero
         */
        void reset() override;
        /**
         * @brief Clear the vibrato
         * @details Not used.
         */
        void clear() override {}
        /**
         * @brief Get the input connection point.
         * @return A shared pointer to the input connection point.
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
         * @brief Check if the noise gate is finished processing
         * @return True if the noise gate is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new noise gate object
         * @param threshold The threshold level in dB
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        static std::shared_ptr<NoiseGate> create(float threshold, float attackTime, float releaseTime, float sampleRate);
    private:
        float threshold;
        float attackTime;
        float releaseTime;
        float sampleRate;
        float envelope = 0.0f;
        float attackCoefficient;
        float releaseCoefficient;
        float thresholdLevel = pow(10.0f, threshold / 20.0f);
};