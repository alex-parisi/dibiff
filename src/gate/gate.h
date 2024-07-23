/// gate.h

#pragma once

namespace dibiff {
    /**
     * @brief Gate Namespace
     * @details The gate namespace contains a collection of simple audio
     * gates that can be applied to an audio signal.
     */
    namespace gate {
        class NoiseGate;
        class ExpanderGate;
        class Ducker;
        class LookaheadGate;
    }
}