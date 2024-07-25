/// level.h

#pragma once

namespace dibiff {
    /**
     * @brief Level Namespace
     * @details The level namespace contains a collection of simple audio
     * objects that can be applied to an audio signal to adjust its level.
     */
    namespace level {
        class Gain;
        class AutomaticGainControl;
        class Mixer;
    }
}