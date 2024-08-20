/// Expander.h

#pragma once

#include "dynamic.h"
#include "../graph/graph.h"

/**
 * @brief Expander
 * @details An expander is a type of dynamic range compressor that expands
 * the amplitude of a signal to a certain threshold. The expander has attack
 * and release times, as well as a makeup gain and knee width. The expander
 * increases the dynamic range of the audio signal by reducing the gain of 
 * signals below a certain threshold, making quiet sounds quieter.
 * @param threshold The threshold of the expander in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the expander in seconds, default value is 0.01
 * @param release The release time of the expander in seconds, default value is 0.1
 * @param ratio The ratio of the expander, default value is 2.0
 * @param kneeWidth The knee width of the expander in dB, default value is calculated
 */
class dibiff::dynamic::Expander : public dibiff::graph::AudioObject {
    public:
        dibiff::graph::AudioInput* input;
        dibiff::graph::AudioOutput* output;
        /**
         * @brief Constructor
         * @details Initializes the expander with default values
         * @param threshold The threshold of the expander in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the expander in seconds, default value is 0.01
         * @param release The release time of the expander in seconds, default value is 0.1
         * @param ratio The ratio of the expander, default value is 2.0
         * @param kneeWidth The knee width of the expander in dB, default value is calculated
         */
        Expander(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> kneeWidth = std::nullopt);
        /**
         * @brief Initialize
         * @details Initializes the expander connection points
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
         * @brief Calculates the static gain characteristic of the expander
         * @details Calculates the static gain characteristic of the expander
         * @param inputdB The input in dB
         * @return The output in dB
         */
        float calculateStaticCharacteristic(float inputdB);
        /**
         * @brief Updates the gain smoothing of the expander
         * @details Updates the gain smoothing of the expander
         * @param xSc The static characteristic of the expander
         * @param inputdB The input in dB
         */
        void updateGainSmoothing(float xSc, float inputdB);
        /**
         * @brief Reset the expander
         * @details Resets the gain smoothing
         */
        void reset() override;
        /**
         * @brief Clear the compressor
         * @details Not used
         */
        void clear() override {}
        /**
         * @brief Check if the expander is finished processing
         * @return True if the expander is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new expander object
         * @param threshold The threshold of the expander in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the expander in seconds, default value is 0.01
         * @param release The release time of the expander in seconds, default value is 0.1
         * @param ratio The ratio of the expander, default value is 2.0
         * @param kneeWidth The knee width of the expander in dB, default value is calculated
         */
        static std::unique_ptr<Expander> create(float& threshold, float& sampleRate, float& attack, float& release, float& ratio, std::optional<std::reference_wrapper<float>> kneeWidth = std::nullopt);
    private:
        float& threshold;
        float& sampleRate;
        float& attack;
        float& release;
        float& ratio;
        std::optional<std::reference_wrapper<float>> knee;
        float gS = 0.0f;
};