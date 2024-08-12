/// Delay.h

#pragma once

#include "time.h"
#include "../graph/graph.h"

/**
 * @brief Delay
 * @details A delay object is a simple object that delays the input signal
 * by a certain amount of time. The delay time is in milliseconds.
 * @param delayTime The delay time of the object in milliseconds
 * @param sampleRate The sample rate of the input signal
 * @return A delay object
 */
class dibiff::time::Delay : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the delay object with a certain delay time
         * @param delayTime The delay time of the object in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        Delay(float delayTime, float sampleRate);
        /**
         * @brief Initialize
         * @details Initializes the delay connection points and buffer
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
         * @brief Reset the delay
         * @details Resets the delay buffer
         */
        void reset() override;
        /**
         * @brief Clear the delay
         * @details Clears the delay buffer
         */
        void clear() override;
        /**
         * @brief Check if the delay is finished processing
         * @return True if the delay is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new delay object
         * @param delayTime The delay time of the object in milliseconds
         * @param sampleRate The sample rate of the input signal
         */
        static std::shared_ptr<Delay> create(float delayTime, float sampleRate);
    private:
        float delayTime;
        float sampleRate;
        std::vector<float> buffer;
        int bufferIndex = 0;
};