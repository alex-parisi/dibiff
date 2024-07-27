/// HighShelfFilter.cpp

#include "HighShelfFilter.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::filter::HighShelfFilter::getName() const { return "HighShelfFilter"; }
/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::HighShelfFilter::HighShelfFilter(float gain, float cutoff, float sampleRate, float qFactor)
: dibiff::filter::DigitalBiquadFilter([&, gain, cutoff, sampleRate, qFactor]() -> dibiff::filter::Coefficients {
    return calculateCoefficients(gain, cutoff, sampleRate, qFactor);
}()), cutoff(cutoff), sampleRate(sampleRate), qFactor(qFactor) {};
/**
 * @brief Calculate the filter coefficients
 * @details Calculates the filter coefficients based on the gain, cutoff frequency, sample rate, and quality factor
 * @param gain The gain of the filter in dB
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
dibiff::filter::Coefficients dibiff::filter::HighShelfFilter::calculateCoefficients(float gain, float cutoff, float sampleRate, float qFactor) {
    float A = std::powf(10.0f, gain / 40.0f);
    float w0 = 2.0f * M_PI * cutoff / sampleRate;
    float alpha = std::sinf(w0) / (2.0f * qFactor);
    float cosw0 = std::cosf(w0);
    float sqrtA = std::sqrtf(A);
    float b0 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha);
    float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0);
    float b2 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha);
    float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha;
    float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0);
    float a2 = (A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha;
    return {b0, b1, b2, a0, a1, a2};
}
/**
 * @brief Set the gain of the filter
 * @param gain The gain of the filter in dB
 */
void dibiff::filter::HighShelfFilter::setGain(float gain) {
    setCoefficients(calculateCoefficients(gain, cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::HighShelfFilter::setCutoff(float cutoff) {
    this->cutoff = cutoff;
    setCoefficients(calculateCoefficients(gain, cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::HighShelfFilter::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
    setCoefficients(calculateCoefficients(gain, cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::HighShelfFilter::setQFactor(float qFactor) {
    this->qFactor = qFactor;
    setCoefficients(calculateCoefficients(gain, cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter
 */
void dibiff::filter::HighShelfFilter::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinhf(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new high shelf filter object
 * @details Creates a new high shelf filter object with custom coefficients
 * @param gain The gain of the filter in dB
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::shared_ptr<dibiff::filter::HighShelfFilter> dibiff::filter::HighShelfFilter::create(float gain, float cutoff, float sampleRate, float qFactor) {
    auto instance = std::make_shared<dibiff::filter::HighShelfFilter>(gain, cutoff, sampleRate, qFactor);
    instance->initialize();
    return instance;
}