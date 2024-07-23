/// Ducker.h

#pragma once

#include "gate.h"
#include "../graph/graph.h"

/**
 * @brief Ducker
 * @details A ducker object is a simple object that attenuates the input signal
 * based on the level of a sidechain input. Uses an external signal to control 
 * the gating process, often used for ducking or creating rhythmic effects. 
 * The ducker object has a certain threshold level, ratio, attack and release times.
 * @param threshold The threshold level of the ducker in dB
 * @param ratio The attenuation ratio of the ducker
 * @param attackTime The attack time of the ducker in milliseconds
 * @param releaseTime The release time of the ducker in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::gate::Ducker : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioReference> reference;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the ducker object with a certain threshold,
         * ratio, attack time, release time, and sample rate
         * @param threshold The threshold level in dB for the sidechain input
         * @param ratio The attenuation ratio (greater than 1)
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        Ducker(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the ducker connection points and envelope
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param input The main input sample
         * @param sidechain The sidechain input sample
         */
        float process(float sample, float reference);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the ducker
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
         * @brief Check if the ducker is finished processing
         * @return True if the ducker is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new ducker object
         * @param threshold The threshold level in dB
         * @param ratio The attenuation ratio (greater than 1)
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        static std::shared_ptr<Ducker> create(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate);
    private:
        float threshold;
        float ratio;
        float attackTime;
        float releaseTime;
        float sampleRate;
        float envelope = 0.0f;
        float attackCoefficient;
        float releaseCoefficient;
        float thresholdLevel;
};