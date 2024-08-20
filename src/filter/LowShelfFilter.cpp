/// LowShelfFilter.cpp

#include "LowShelfFilter.h"

/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::LowShelfFilter::LowShelfFilter(float& gain, float& cutoff, float& sampleRate, float& qFactor)
: _gain(gain), _cutoff(cutoff), _sampleRate(sampleRate), _qFactor(qFactor), 
  dibiff::filter::DigitalBiquadFilter(nullptr) {
    name = "LowShelfFilter";
    calculateCoefficients();
    setCoefficients(&coeffs);
}
/**
 * @brief Calculate the filter coefficients
 * @details Calculates the filter coefficients based on the gain, cutoff frequency, sample rate, and quality factor
 * @param gain The gain of the filter in dB
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::LowShelfFilter::calculateCoefficients() {
    float A = std::pow(10.0f, _gain / 40.0f);
    float w0 = 2.0f * M_PI * _cutoff / _sampleRate;
    float alpha = std::sin(w0) / (2.0f * _qFactor);
    float cosw0 = std::cos(w0);
    float sqrtA = std::sqrt(A);
    float b0 = A * ((A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha);
    float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0);
    float b2 = A * ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha);
    float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha;
    float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0);
    float a2 = (A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha;
    coeffs = dibiff::filter::Coefficients{b0, b1, b2, a0, a1, a2};
}
/**
 * @brief Set the gain of the filter
 * @param gain The gain of the filter in dB
 */
void dibiff::filter::LowShelfFilter::setGain(float gain) {
    _gain = gain;
    calculateCoefficients();
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::LowShelfFilter::setCutoff(float cutoff) {
    _cutoff = cutoff;
    calculateCoefficients();
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::LowShelfFilter::setSampleRate(float sampleRate) {
    _sampleRate = sampleRate;
    calculateCoefficients();
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::LowShelfFilter::setQFactor(float qFactor) {
    _qFactor = qFactor;
    calculateCoefficients();
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter
 */
void dibiff::filter::LowShelfFilter::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinh(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new low shelf filter object
 * @details Creates a new low shelf filter object with custom coefficients
 * @param gain The gain of the filter in dB
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::unique_ptr<dibiff::filter::LowShelfFilter> dibiff::filter::LowShelfFilter::create(float& gain, float& cutoff, float& sampleRate, float& qFactor) {
    auto instance = std::make_unique<dibiff::filter::LowShelfFilter>(gain, cutoff, sampleRate, qFactor);
    instance->initialize();
    return std::move(instance);
}