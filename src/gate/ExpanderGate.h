/// ExpanderGate.h

#pragma once

#include "gate.h"
#include "../graph/graph.h"

/**
 * @brief Expander Gate
 * @details An expander gate object is a simple object that expands the dynamic
 * range of the input signal based on a certain threshold level. Similar to a 
 * noise gate but reduces the level of audio signals more smoothly. It expands 
 * the dynamic range by attenuating signals below a threshold.The expander
 * gate object has a certain ratio, attack and release times.
 * @param threshold The threshold level of the expander gate in dB
 * @param ratio The expansion ratio of the expander gate
 * @param attackTime The attack time of the expander gate in milliseconds
 * @param releaseTime The release time of the expander gate in milliseconds
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::gate::ExpanderGate : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the expander gate object with a certain threshold,
         * ratio, attack time, release time, and sample rate
         * @param threshold The threshold level in dB
         * @param ratio The expansion ratio (greater than 1)
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        ExpanderGate(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the expander gate connection points and envelope
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param input The input sample
         * @return The output sample
         */
        float process(float sample);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         * @param buffer The input buffer
         * @param blockSize The size of the block
         */
        void process() override;
        /**
         * @brief Reset the expander gate
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
         * @brief Check if the expander gate is finished processing
         * @return True if the expander gate is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new expander gate object
         * @param threshold The threshold level in dB
         * @param ratio The expansion ratio (greater than 1)
         * @param attackTime The attack time in milliseconds
         * @param releaseTime The release time in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        static std::shared_ptr<ExpanderGate> create(float threshold, float ratio, float attackTime, float releaseTime, float sampleRate);
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