/// AutomaticGainControl.h

#pragma once

#include "level.h"
#include "../graph/graph.h"

/**
 * @brief Automatic Gain Control
 * @details An automatic gain control (AGC) object is a simple object that adjusts
 * the gain of an audio signal to a certain target level. The AGC has attack and
 * release times, as well as a coefficient for RMS level calculation.
 * @param targetLevel The target output level in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the AGC in seconds, default value is 0.01
 * @param release The release time of the AGC in seconds, default value is 0.1
 * @param rmsCoefficient The coefficient for RMS level calculation, default value is 0.999
 */
class dibiff::level::AutomaticGainControl : public dibiff::graph::AudioObject {
    public:
        dibiff::graph::AudioInput* input;
        dibiff::graph::AudioOutput* output;
        /**
         * @brief Constructor
         * @details Initializes the AGC with given parameters
         * @param targetLevel The target output level in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the AGC in seconds, default value is 0.01
         * @param release The release time of the AGC in seconds, default value is 0.1
         * @param rmsCoefficient The coefficient for RMS level calculation, default value is 0.999
         */
        AutomaticGainControl(float& targetLevel, float& sampleRate, float& attack, float& release, float& rmsCoefficient);
        /**
         * @brief Initialize
         * @details Initializes the AGC connection points and parameters
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param sample The input sample
         */
        float process(float sample);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the AGC
         * @details Resets the AGC to the default state
         */
        void reset() override;
        /**
         * @brief Clear the compressor
         * @details Not used
         */
        void clear() override {}
        /**
         * @brief Check if the AGC is finished processing
         * @return True if the AGC is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new AGC object
         * @param targetLevel The target output level in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the AGC in seconds, default value is 0.01
         * @param release The release time of the AGC in seconds, default value is 0.1
         * @param rmsCoefficient The coefficient for RMS level calculation, default value is 0.999
         */
        static std::unique_ptr<AutomaticGainControl> create(float& targetLevel, float& sampleRate, float& attack, float& release, float& rmsCoefficient);
    private:
        float& targetLevel;
        float& sampleRate;
        float& attack;
        float& release;
        float& rmsCoefficient;
        float attackCoefficient;
        float releaseCoefficient;
        float targetLevelLinear;
        float currentGain = 1.0f;
        float rmsLevel = 0.0f;
};