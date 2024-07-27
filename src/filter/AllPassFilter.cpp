/// AllPassFilter.cpp

#include "AllPassFilter.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::filter::AllPassFilter::getName() const { return "AllPassFilter"; }
/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::AllPassFilter::AllPassFilter(float cutoff, float sampleRate, float qFactor)
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
dibiff::filter::Coefficients dibiff::filter::AllPassFilter::calculateCoefficients(float cutoff, float sampleRate, float qFactor) {
    float w0 = 2.0f * M_PI * cutoff / sampleRate;
    float alpha = std::sinf(w0) / (2.0f * qFactor);
    float b0 = 1.0f - alpha;
    float b1 = -2.0f * std::cosf(w0);
    float b2 = 1.0f + alpha;
    return {b0, b1, b2, b2, b1, b0};
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::AllPassFilter::setCutoff(float cutoff) {
    this->cutoff = cutoff;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::AllPassFilter::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::AllPassFilter::setQFactor(float qFactor) {
    this->qFactor = qFactor;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter 
 */
void dibiff::filter::AllPassFilter::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinhf(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new all pass filter object
 * @details Creates a new all pass filter object with custom coefficients
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::shared_ptr<dibiff::filter::AllPassFilter> dibiff::filter::AllPassFilter::create(float cutoff, float sampleRate, float qFactor) {
    auto instance = std::make_shared<dibiff::filter::AllPassFilter>(cutoff, sampleRate, qFactor);
    instance->initialize();
    return instance;
}