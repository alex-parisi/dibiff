/// PeakingEQFilter.h

#pragma once

#include "filter.h"
#include "DigitalBiquadFilter.h"

/**
 * @brief Peaking EQ Filter
 * @details A peaking EQ filter is a filter that boosts or cuts a certain
 * frequency range with a constant Q factor.
 * @param gain The gain of the filter in dB
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter, default value is 0.7071067811865476, or 1/sqrt(2)
 * @return A peaking EQ filter object
 * @see DigitalBiquadFilter
 */
class dibiff::filter::PeakingEQFilter : public dibiff::filter::DigitalBiquadFilter {
    public:
        /**
         * @brief Constructor
         * @details Initializes the filter with default values
         */
        PeakingEQFilter(float& gain, float& cutoff, float& sampleRate, float& qFactor);
        /**
         * @brief Calculate the filter coefficients
         * @details Calculates the filter coefficients based on the gain, cutoff frequency, sample rate, and quality factor
         * @param gain The gain of the filter in dB
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        void calculateCoefficients();
        /**
         * @brief Set the gain of the filter
         * @param gain The gain of the filter in dB
         */
        void setGain(float gain);
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
         * @brief Create a new peaking EQ filter object
         * @details Creates a new peaking EQ filter object with custom coefficients
         * @param gain The gain of the filter in dB
         * @param cutoff The cutoff frequency of the filter
         * @param sampleRate The sample rate of the input signal
         * @param qFactor The quality factor of the filter
         */
        static std::unique_ptr<PeakingEQFilter> create(float& gain, float& cutoff, float& sampleRate, float& qFactor);
    private:
        float& _gain;
        float& _cutoff;
        float& _sampleRate;
        float& _qFactor;
        dibiff::filter::Coefficients coeffs;
};