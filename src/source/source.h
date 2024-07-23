/// source.h

#pragma once

namespace dibiff {
    /**
     * @brief Source Namespace
     * @details The source namespace contains a collection of simple audio
     * source objects that can be used to generate audio signals at specified levels.
     */
    namespace source {
        class SineSource;
        class TriangleSource;
        class SquareSource;
        class WhiteNoiseSource;
        class PinkNoiseSource;
    }
}