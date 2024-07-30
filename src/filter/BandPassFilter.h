/// BandPassFilter.h

#pragma once

#include "filter.h"
#include "DigitalBiquadFilter.h"

/**
 * @brief Band Pass Filter
 * @details A band pass filter is a filter that passes signals with a
 * frequency within a certain range and attenuates signals with frequencies
 * outside of that range. The filter has a constant skirt gain.
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter, default value is 0.7071067811865476, or 1/sqrt(2)
 * @return A band pass filter object
 * @see DigitalBiquadFilter
 */
class dibiff::filter::BandPassFilterConstantSkirtGain : public dibiff::filter::DigitalBiquadFilter {
    public:
        /**
         * @brief Constructor
         * @details Initializes the filter with default values
         */
        BandPassFilterConstantSkirtGain(float cutoff, float sampleRate, float qFactor = 0.7071067811865476f);
        /**
         * @brief Calculate the filter coefficients
         * @details Calculates the filter coefficients based on the cutoff frequency, sample rate, and quality factor
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        dibiff::filter::Coefficients calculateCoefficients(float cutoff, float sampleRate, float qFactor);
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
         * @brief Create a new band pass filter object
         * @details Creates a new band pass filter object with custom coefficients
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        static std::shared_ptr<BandPassFilterConstantSkirtGain> create(float cutoff, float sampleRate, float qFactor = 0.7071067811865476f);
    private:
        float cutoff;
        float sampleRate;
        float qFactor;
};
/**
 * @brief Band Pass Filter
 * @details A band pass filter is a filter that passes signals with a
 * frequency within a certain range and attenuates signals with frequencies
 * outside of that range. The filter has a constant peak gain.
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter, default value is 0.7071067811865476, or 1/sqrt(2)
 * @return A band pass filter object
 * @see DigitalBiquadFilter
 */
class dibiff::filter::BandPassFilterConstantPeakGain : public dibiff::filter::DigitalBiquadFilter {
    public:
        /**
         * @brief Constructor
         * @details Initializes the filter with default values
         */
        BandPassFilterConstantPeakGain(float cutoff, float sampleRate, float qFactor = 0.7071067811865476f);
        /**
         * @brief Calculate the filter coefficients
         * @details Calculates the filter coefficients based on the cutoff frequency, sample rate, and quality factor
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        dibiff::filter::Coefficients calculateCoefficients(float cutoff, float sampleRate, float qFactor);
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
         * @brief Create a new band pass filter object
         * @details Creates a new band pass filter object with custom coefficients
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        static std::shared_ptr<BandPassFilterConstantPeakGain> create(float cutoff, float sampleRate, float qFactor = 0.7071067811865476f);
    private:
        float cutoff;
        float sampleRate;
        float qFactor;
};