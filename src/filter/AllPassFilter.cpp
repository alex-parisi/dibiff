/// AllPassFilter.cpp

#include "AllPassFilter.h"

/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::AllPassFilter::AllPassFilter(float& cutoff, float& sampleRate, float& qFactor) 
: _cutoff(cutoff), _sampleRate(sampleRate), _qFactor(qFactor), 
  dibiff::filter::DigitalBiquadFilter(nullptr) {
    name = "AllPassFilter";
    calculateCoefficients();
    setCoefficients(&coeffs);
}

/**
 * @brief Calculate the filter coefficients
 * @details Calculates the filter coefficients based on the cutoff frequency, sample rate, and quality factor
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::AllPassFilter::calculateCoefficients() {
    float w0 = 2.0f * M_PI * _cutoff / _sampleRate;
    float alpha = std::sin(w0) / (2.0f * _qFactor);
    float b0 = 1.0f - alpha;
    float b1 = -2.0f * std::cos(w0);
    float b2 = 1.0f + alpha;
    coeffs = dibiff::filter::Coefficients{b0, b1, b2, b2, b1, b0};
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::AllPassFilter::setCutoff(float cutoff) {
    _cutoff = cutoff;
    calculateCoefficients();
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::AllPassFilter::setSampleRate(float sampleRate) {
    _sampleRate = sampleRate;
    calculateCoefficients();
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::AllPassFilter::setQFactor(float qFactor) {
    _qFactor = qFactor;
    calculateCoefficients();
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter 
 */
void dibiff::filter::AllPassFilter::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinh(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new all pass filter object
 * @details Creates a new all pass filter object with custom coefficients
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::unique_ptr<dibiff::filter::AllPassFilter> dibiff::filter::AllPassFilter::create(float& cutoff, float& sampleRate, float& qFactor) {
    auto instance = std::make_unique<dibiff::filter::AllPassFilter>(cutoff, sampleRate, qFactor);
    instance->initialize();
    return std::move(instance);
}