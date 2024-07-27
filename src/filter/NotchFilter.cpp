/// NotchFilter.cpp

#include "NotchFilter.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::filter::NotchFilter::getName() const { return "NotchFilter"; }
/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::NotchFilter::NotchFilter(float cutoff, float sampleRate, float qFactor)
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
dibiff::filter::Coefficients dibiff::filter::NotchFilter::calculateCoefficients(float cutoff, float sampleRate, float qFactor) {
    float w0 = 2.0f * M_PI * cutoff / sampleRate;
    float alpha = std::sin(w0) / (2.0f * qFactor);
    float b1 = -2.0f * std::cosf(w0);
    float a0 = 1.0f + alpha;
    float a2 = 1.0f - alpha;
    return {1.0f, b1, 1.0f, a0, b1, a2};
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::NotchFilter::setCutoff(float cutoff) {
    this->cutoff = cutoff;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::NotchFilter::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::NotchFilter::setQFactor(float qFactor) {
    this->qFactor = qFactor;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter
 */
void dibiff::filter::NotchFilter::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinh(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new notch filter object
 * @details Creates a new notch filter object with custom coefficients
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::shared_ptr<dibiff::filter::NotchFilter> dibiff::filter::NotchFilter::create(float cutoff, float sampleRate, float qFactor) {
    auto instance = std::make_shared<dibiff::filter::NotchFilter>(cutoff, sampleRate, qFactor);
    instance->initialize();
    return instance;
}