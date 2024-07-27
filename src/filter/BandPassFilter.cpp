/// BandPassFilter.cpp

#include "BandPassFilter.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::filter::BandPassFilterConstantSkirtGain::getName() const { return "BandPassFilterConstantSkirtGain"; }
/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::BandPassFilterConstantSkirtGain::BandPassFilterConstantSkirtGain(float cutoff, float sampleRate, float qFactor)
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
dibiff::filter::Coefficients dibiff::filter::BandPassFilterConstantSkirtGain::calculateCoefficients(float cutoff, float sampleRate, float qFactor) {
    float w0 = 2.0f * M_PI * cutoff / sampleRate;
    float alpha = std::sin(w0) / (2.0f * qFactor);
    float b0 = qFactor * alpha;
    float a0 = 1 + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 = 1 - alpha;
    return {b0, 0.0f, -b0, a0, a1, a0};
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::BandPassFilterConstantSkirtGain::setCutoff(float cutoff) {
    this->cutoff = cutoff;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::BandPassFilterConstantSkirtGain::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::BandPassFilterConstantSkirtGain::setQFactor(float qFactor) {
    this->qFactor = qFactor;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter
 */
void dibiff::filter::BandPassFilterConstantSkirtGain::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinhf(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new band pass filter object
 * @details Creates a new band pass filter object with custom coefficients
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::shared_ptr<dibiff::filter::BandPassFilterConstantSkirtGain> dibiff::filter::BandPassFilterConstantSkirtGain::create(float cutoff, float sampleRate, float qFactor) {
    auto instance = std::make_shared<dibiff::filter::BandPassFilterConstantSkirtGain>(cutoff, sampleRate, qFactor);
    instance->initialize();
    return instance;
}

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::filter::BandPassFilterConstantPeakGain::getName() const { return "BandPassFilterConstantPeakGain"; }
/**
 * @brief Constructor
 * @details Initializes the filter with default values
 */
dibiff::filter::BandPassFilterConstantPeakGain::BandPassFilterConstantPeakGain(float cutoff, float sampleRate, float qFactor)
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
dibiff::filter::Coefficients dibiff::filter::BandPassFilterConstantPeakGain::calculateCoefficients(float cutoff, float sampleRate, float qFactor) {
    float w0 = 2.0f * M_PI * cutoff / sampleRate;
    float alpha = std::sin(w0) / (2.0f * qFactor);
    float b0 = alpha;
    float a0 = 1 + alpha;
    float a1 = -2.0f * std::cos(w0);
    float a2 = 1 - alpha;
    return {b0, 0.0f, -b0, a0, a1, a0};
}
/**
 * @brief Set the cutoff frequency of the filter
 * @param cutoff The cutoff frequency of the filter
 */
void dibiff::filter::BandPassFilterConstantPeakGain::setCutoff(float cutoff) {
    this->cutoff = cutoff;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the sample rate of the input signal
 * @param sampleRate The sample rate of the input signal
 */
void dibiff::filter::BandPassFilterConstantPeakGain::setSampleRate(float sampleRate) {
    this->sampleRate = sampleRate;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the quality factor of the filter
 * @param qFactor The quality factor of the filter
 */
void dibiff::filter::BandPassFilterConstantPeakGain::setQFactor(float qFactor) {
    this->qFactor = qFactor;
    setCoefficients(calculateCoefficients(cutoff, sampleRate, qFactor));
}
/**
 * @brief Set the bandwidth of the filter
 * @param bandwidth The bandwidth of the filter
 */
void dibiff::filter::BandPassFilterConstantPeakGain::setBandwidth(float bandwidth) {
    float Q = 1.0f / (2.0f * std::sinhf(bandwidth * std::log10(2.0f) / 2.0f));
    setQFactor(Q);
}
/**
 * @brief Create a new band pass filter object
 * @details Creates a new band pass filter object with custom coefficients
 * @param cutoff The cutoff frequency of the filter
 * @param sampleRate The sample rate of the input signal
 * @param qFactor The quality factor of the filter
 */
std::shared_ptr<dibiff::filter::BandPassFilterConstantPeakGain> dibiff::filter::BandPassFilterConstantPeakGain::create(float cutoff, float sampleRate, float qFactor) {
    auto instance = std::make_shared<dibiff::filter::BandPassFilterConstantPeakGain>(cutoff, sampleRate, qFactor);
    instance->initialize();
    return instance;
}