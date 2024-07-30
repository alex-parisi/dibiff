/// Limiter.h

#pragma once

#include "dynamic.h"
#include "../graph/graph.h"

/**
 * @brief Limiter
 * @details A limiter is a type of dynamic range compressor that limits the
 * amplitude of a signal to a certain threshold. The limiter has attack and
 * release times, as well as a makeup gain and knee width.
 * @param threshold The threshold of the limiter in dB
 * @param sampleRate The sample rate of the input signal
 * @param attack The attack time of the limiter in seconds, default value is 0.01
 * @param release The release time of the limiter in seconds, default value is 0.1
 * @param makeupGain The makeup gain of the limiter in dB, default value is calculated
 * @param kneeWidth The knee width of the limiter in dB, default value is calculated
 * @return A limiter object
 */
class dibiff::dynamic::Limiter : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public: 
        /**
         * @brief Constructor
         * @details Initializes the limiter with default values
         * @param threshold The threshold of the limiter in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the limiter in seconds, default value is 0.01
         * @param release The release time of the limiter in seconds, default value is 0.1
         * @param makeupGain The makeup gain of the limiter in dB, default value is calculated
         * @param kneeWidth The knee width of the limiter in dB, default value is calculated
         * 
         */
        Limiter(float threshold, float sampleRate, float attack, float release, std::optional<float> makeupGain, std::optional<float> kneeWidth);
        /**
         * @brief Initialize
         * @details Initializes the limiter connection points and makeup gain
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
         * @brief Calculates the static gain characteristic of the limiter
         * @details Calculates the static gain characteristic of the limiter
         * @param inputdB The input in dB
         * @return The output in dB
         */
        float calculateStaticCharacteristic(float inputdB);
        /**
         * @brief Calculates the gain smoothing of the limiter
         * @details Calculates the gain smoothing of the limiter
         * @param xSc The static characteristic of the limiter
         * @param inputdB The input in dB
         */
        void updateGainSmoothing(float xSc, float inputdB);
        /**
         * @brief Reset the limiter
         * @details Resets the gain smoothing
         */
        void reset() override;
        /**
         * @brief Clear the limiter
         * @details Not used
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
         * @brief Check if the limiter is finished processing
         * @return True if the limiter is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new limiter object
         * @param threshold The threshold of the limiter in dB
         * @param sampleRate The sample rate of the input signal
         * @param attack The attack time of the limiter in seconds, default value is 0.01
         * @param release The release time of the limiter in seconds, default value is 0.1
         * @param makeupGain The makeup gain of the limiter in dB, default value is calculated
         * @param kneeWidth The knee width of the limiter in dB, default value is calculated
         */
        static std::shared_ptr<Limiter> create(float threshold, float sampleRate, float attack, float release, std::optional<float> makeupGain, std::optional<float> kneeWidth);
    private:
        float release;
        float attack;
        float threshold;
        float sampleRate;
        std::optional<float> makeupGain;
        std::optional<float> knee;
        float gS = 0.0f;
};