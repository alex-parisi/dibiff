/// LowPassFilter.h

#pragma once

#include "filter.h"
#include "DigitalBiquadFilter.h"

/**
 * @brief Low Pass Filter
 * @details A low pass filter is a filter that passes signals with a 
 * frequency lower than a certain cutoff frequency and attenuates signals 
 * with frequencies higher than the cutoff frequency.
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter, default value is 0.7071067811865476, or 1/sqrt(2)
 * @return A low pass filter object
 * @see DigitalBiquadFilter
 */
class dibiff::filter::LowPassFilter : public dibiff::filter::DigitalBiquadFilter {
    public:
        /**
         * @brief Constructor
         * @details Initializes the filter with default values
         */
        LowPassFilter(float& cutoff, float& sampleRate, float& qFactor);
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
         * @brief Create a new low pass filter object
         * @details Creates a new low pass filter object with custom coefficients
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        static std::shared_ptr<LowPassFilter> create(float& cutoff, float& sampleRate, float& qFactor);
    private:
        float& _cutoff;
        float& _sampleRate;
        float& _qFactor;
        dibiff::filter::Coefficients coeffs;
};