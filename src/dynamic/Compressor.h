/// Compressor.h

#pragma once

#include "dynamic.h"
#include "../graph/graph.h"

/**
 * @brief Compressor
 * @details A compressor is a type of dynamic range compressor that compresses
 * the amplitude of a signal to a certain threshold. The compressor has attack
 * and release times, as well as a makeup gain and knee width.
 * @param threshold The threshold of the compressor in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the compressor in seconds, default value is 0.01
 * @param release The release time of the compressor in seconds, default value is 0.1
 * @param ratio The ratio of the compressor, default value is 2.0
 * @param makeupGain The makeup gain of the compressor in dB, default value is calculated
 * @param kneeWidth The knee width of the compressor in dB, default value is calculated
 * @return A compressor object
 */
class dibiff::dynamic::Compressor : public dibiff::graph::AudioObject {
    public:
        dibiff::graph::AudioInput* input;
        dibiff::graph::AudioOutput* output;
        /**
         * @brief Constructor
         * @details Initializes the compressor with default values
         * @param threshold The threshold of the compressor in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the compressor in seconds, default value is 0.01
         * @param release The release time of the compressor in seconds, default value is 0.1
         * @param ratio The ratio of the compressor, default value is 2.0
         * @param makeupGain The makeup gain of the compressor in dB, default value is calculated
         * @param kneeWidth The knee width of the compressor in dB, default value is calculated
         * 
         */
        Compressor(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> makeupGain = std::nullopt, std::optional<std::reference_wrapper<float>> kneeWidth = std::nullopt);
        /**
         * @brief Initialize
         * @details Initializes the compressor connection points and makeup gain
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
         * @brief Calculates the static gain characteristic of the compressor
         * @details Calculates the static gain characteristic of the compressor
         * @param inputdB The input in dB
         * @return The output in dB
         */
        float calculateStaticCharacteristic(float inputdB);
        /**
         * @brief Calculates the gain smoothing of the compressor
         * @details Calculates the gain smoothing of the compressor
         * @param xSc The static characteristic of the compressor
         * @param inputdB The input in dB
         */
        void updateGainSmoothing(float xSc, float inputdB);
        /**
         * @brief Reset the compressor
         * @details Resets the gain smoothing
         */
        void reset() override;
        /**
         * @brief Clear the compressor
         * @details Not used
         */
        void clear() override {}
        /**
         * @brief Check if the compressor is finished processing
         * @return True if the compressor is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new compressor object
         * @param threshold The threshold of the compressor in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the compressor in seconds, default value is 0.01
         * @param release The release time of the compressor in seconds, default value is 0.1
         * @param ratio The ratio of the compressor, default value is 2.0
         * @param makeupGain The makeup gain of the compressor in dB, default value is calculated
         * @param kneeWidth The knee width of the compressor in dB, default value is calculated
         */
        static std::unique_ptr<Compressor> create(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> makeupGain = std::nullopt, std::optional<std::reference_wrapper<float>> kneeWidth = std::nullopt);
    private:
        float& release;
        float& attack;
        float& threshold;
        float& sampleRate;
        float& ratio;
        std::optional<std::reference_wrapper<float>> makeupGain;
        std::optional<std::reference_wrapper<float>> knee;
        float gS = 0.0f;
        float _makeupGain = 0.0f;
};