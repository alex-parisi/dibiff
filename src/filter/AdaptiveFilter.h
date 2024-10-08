/// AdaptiveFilter.h

#pragma once

#include "filter.h"
#include "DigitalBiquadFilter.h"

/**
 * @brief Adaptive Filter (LMS)
 * @details An adaptive filter is a filter that adjusts its coefficients
 * based on the input signal. It is commonly used in noise cancellation
 * and echo cancellation applications.
 * @param filterLength The length of the filter
 * @param stepSize The step size of the filter
 */
class dibiff::filter::AdaptiveFilter : public dibiff::graph::AudioObject {
    public: 
        dibiff::graph::AudioInput* input;
        dibiff::graph::AudioInput* reference;
        dibiff::graph::AudioOutput* output;
        /**
         * @brief Constructor
         * @details Initializes the adaptive filter with the specified length
         * and step size
         * @param filterLength The length of the filter
         * @param stepSize The step size of the filter
         */
        AdaptiveFilter(int& filterLength, float& stepSize);
        /**
         * @brief Initialize
         * @details Initializes the adaptive filter connection points
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param sample The input sample
         * @param reference The reference sample
         */
        float process(float sample, float reference);
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the filter
         * @details Resets the filter coefficients and buffer
         */
        void reset() override;
        /**
         * @brief Clear the filter
         * @details Clears the buffer
         */
        void clear() override;
        /**
         * @brief Check if the filter is finished processing
         * @return True if the filter is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new adaptive filter object
         * @param filterLength The length of the filter
         * @param stepSize The step size of the filter
         */
        static std::unique_ptr<AdaptiveFilter> create(int& filterLength, float& stepSize);
    private:
        int& filterLength;
        float& stepSize;
        std::vector<float> filterCoefficients;
        std::vector<float> buffer;
};