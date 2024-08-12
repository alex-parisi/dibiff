/// DigitalBiquadFilter.h

#pragma once

#include "../graph/graph.h"
#include "filter.h"

/** 
 * @brief Digital Biquad Filter
 * @details A digital biquad filter is a type of infinite impulse response filter.
 * The transfer function is defined by the following equation:
 * H(z) = (b0 + b1*z^-1 + b2*z^-2) / (a0 + a1*z^-1 + a2*z^-2)
 * The filter is implemented by applying the following difference equation:
 * y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
 */
class dibiff::filter::DigitalBiquadFilter : public dibiff::graph::AudioObject {
    public: 
        std::shared_ptr<dibiff::graph::AudioInput> input;
        std::shared_ptr<dibiff::graph::AudioOutput> output;
        /**
         * @brief Constructor
         * @details Initializes the filter with custom coefficients
         * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
         * b0, b1 b2, a0, a1, a2
         * @see reset
         * @see dibiff::Coefficients
         */
        DigitalBiquadFilter(dibiff::filter::Coefficients& coeffs);
        /**
         * @brief Initialize
         * @details Initializes the filter state variables and connection points
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
         * @details Processes a block of samples of audio data
         */
        void process() override;
        /**
         * @brief Set the filter coefficients
         * @details Sets the filter coefficients to custom values
         * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
         * b0, b1 b2, a0, a1, a2
         * @see dibiff::Coefficients 
         */
        void setCoefficients(dibiff::filter::Coefficients& coeffs);
        /**
         * @brief Reset the filter
         * @details Resets the filter state variables
         */
        void reset() override;
        /**
         * @brief Clear the filter
         * @details Not implemented.
         */
        void clear() override {};
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
         * @brief Create a new filter object
         * @details Creates a new filter object with custom coefficients
         * @param coeffs A dibiff::Coefficients struct containing the filter coefficients:
         * b0, b1 b2, a0, a1, a2
         */
        static std::shared_ptr<DigitalBiquadFilter> create(dibiff::filter::Coefficients& coeffs);
        /**
         * @brief Destructor
         * @details Destroys the filter object
         */
        ~DigitalBiquadFilter() {};
    private:
        dibiff::filter::Coefficients& _coeffs;
        float x1, x2, y1, y2;
        long int iter;
};