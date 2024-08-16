/// AcousticEchoCanceller.h

#pragma once

#include "adaptive.h"
#include "../graph/graph.h"
#include "../filter/AdaptiveFilter.h"

/**
 * @brief Acoustic Echo Canceller
 * @details An acoustic echo canceller object is a simple object that removes
 * the echo from the input signal using an adaptive filter. The acoustic echo
 * canceller object has a certain filter length, step size, and sample rate.
 * @param filterLength The length of the adaptive filter
 * @param stepSize The step size of the adaptive filter
 * @param sampleRate The sample rate of the input signal
 */
class dibiff::adaptive::AcousticEchoCanceller : public dibiff::graph::AudioObject {
    public:
        std::shared_ptr<dibiff::graph::AudioInput> input;
        std::shared_ptr<dibiff::graph::AudioInput> reference;
        std::shared_ptr<dibiff::graph::AudioOutput> output;
        /**
         * @brief Constructor
         * @details Initializes the acoustic echo canceller with a certain filter length,
         * step size, buffer size, and sample rate
         * @param filterLength The length of the adaptive filter
         * @param stepSize The step size of the adaptive filter
         */
        AcousticEchoCanceller(int& filterLength, float& stepSize);
        /**
         * @brief Initialize
         * @details Initializes the acoustic echo canceller connection points and adaptive filter
         */
        void initialize() override;
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         * @param inputBuffer The input buffer
         * @param referenceBuffer The reference buffer
         * @param blockSize The size of the block
         */
        void process() override;
        /**
         * @brief Reset the acoustic echo canceller
         * @details Resets the adaptive filter
         */
        void reset() override;
        /**
         * @brief Clear the acoustic echo canceller
         * @details Clears the adaptive filter
         */
        void clear() override;
        /**
         * @brief Check if the AEC is finished processing
         * @return True if the AEC is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new acoustic echo canceller object
         * @param filterLength The length of the adaptive filter
         * @param stepSize The step size of the adaptive filter
         */
        static std::shared_ptr<AcousticEchoCanceller> create(int& filterLength, float& stepSize);
    private:
        std::shared_ptr<dibiff::filter::AdaptiveFilter> adaptiveFilter;
        int& filterLength;
        float& stepSize;
};