/// HighPassFilter.cpp

#include "HighPassFilter.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::filter::HighPassFilter::getName() const { return "HighPassFilter"; }
/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::HighPassFilter::HighPassFilter(float cutoff, float sampleRate, float qFactor)
: dibiff::filter::DigitalBiquadFilter([&, cutoff, sampleRate, qFactor]() -> dibiff::filter::Coefficients {
    return calculateCoefficients(cutoff, sampleRate, qFactor);
}()), cutoff(cutoff), sampleRate(sampleRate), qFactor(qFactor) {};
/**
 * @brief Calculate the filter coefficients
 * @details Calculates the filter coefficients based on the cutoff frequency, sample rate, and quality factor
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
dibiff::filter::Coefficients dibiff::filter::HighPassFilter::calculateCoefficients(float cutoff, float sampleRate, float qFactor) {
    float w0 = 2.0f * M_PI * cutoff / sampleRate;
    float cosw0 = std::cosf(w0);
    float alpha = std::sinf(w0) / (2.0f * qFactor);
    float b1 = -(1.0f + cosw0);
    float b0 = -b1 / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha;
    return {b0, b1, b0, a0, a1, a2};
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::HighPassFilter::setCutoff(float cutoff) {
    this->cutoff = cutoff;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::HighPassFilter::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::HighPassFilter::setQFactor(float qFactor) {
    this->qFactor = qFactor;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter
 */
void dibiff::filter::HighPassFilter::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinhf(bandwidth * std::log10f(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new high pass filter object
 * @details Creates a new high pass filter object with custom coefficients
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::shared_ptr<dibiff::filter::HighPassFilter> dibiff::filter::HighPassFilter::create(float cutoff, float sampleRate, float qFactor) {
    auto instance = std::make_shared<dibiff::filter::HighPassFilter>(cutoff, sampleRate, qFactor);
    instance->initialize();
    return instance;
}