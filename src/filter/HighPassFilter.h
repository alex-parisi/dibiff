/// HighPassFilter.h

#pragma once

#include "filter.h"
#include "DigitalBiquadFilter.h"

/**
 * @brief High Pass Filter
 * @details A high pass filter is a filter that passes signals with a 
 * frequency higher than a certain cutoff frequency and attenuates signals 
 * with frequencies lower than the cutoff frequency.
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter, default value is 0.7071067811865476, or 1/sqrt(2)
 * @return A high pass filter object
 * @see DigitalBiquadFilter
 */
class dibiff::filter::HighPassFilter : public dibiff::filter::DigitalBiquadFilter {
    public:
        /**
         * @brief Constructor
         * @details Initializes the filter with default values
         */
        HighPassFilter(float& cutoff, float& sampleRate, float& qFactor);
        /**
         * @brief Calculate the filter coefficients
         * @details Calculates the filter coefficients based on the cutoff frequency, sample rate, and quality factor
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        void calculateCoefficients();
        /**
         * @brief Set the cutoff frequency of the filter
         * @param cutoff The cutoff frequency of the filter
         */
        void setCutoff(float cutoff);
        /**
         * @brief Set the sample rate of the input signal
         * @param sampleRate The sample rate of the input signal
         */
        void setSampleRate(float sampleRate);
        /**
         * @brief Set the quality factor of the filter
         * @param qFactor The quality factor of the filter
         */
        void setQFactor(float qFactor);
        /**
         * @brief Set the bandwidth of the filter
         * @param bandwidth The bandwidth of the filter
         */
        void setBandwidth(float bandwidth);
        /**
         * @brief Create a new high pass filter object
         * @details Creates a new high pass filter object with custom coefficients
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        static std::unique_ptr<HighPassFilter> create(float& cutoff, float& sampleRate, float& qFactor);
    private:
        float& _cutoff;
        float& _sampleRate;
        float& _qFactor;
        dibiff::filter::Coefficients coeffs;
};