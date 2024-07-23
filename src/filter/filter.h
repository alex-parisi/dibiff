/// filter.h

#pragma once

namespace dibiff {
    /**
     * @brief Filter Namespace
     * @details A namespace containing a set of digital biquad filter
     * for audio processing
     * @see DigitalBiquadFilter
     */
    namespace filter {
        struct Coefficients;
        class DigitalBiquadFilter;
        class LowPassFilter;
        class HighPassFilter;
        class BandPassFilterConstantSkirtGain;
        class BandPassFilterConstantPeakGain;
        class NotchFilter;
        class AllPassFilter;
        class PeakingEQFilter;
        class LowShelfFilter;
        class HighShelfFilter;
        class AdaptiveFilter;
        class PinkNoiseFilter;
    }
}
/**
 * @brief Coefficients Struct
 * @details A struct containing the coefficients of a digital biquad filter
 */
struct dibiff::filter::Coefficients {
    float b0, b1, b2, a0, a1, a2;
};
