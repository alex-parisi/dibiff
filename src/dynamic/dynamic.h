/// dynamic.h

#pragma once

namespace dibiff {
    /**
     * @brief Dynamic Range Compressors
     * @details Dynamic range compressors are audio processors that reduce the dynamic
     * range of an audio signal. They are used to control the volume of an audio signal
     * and are commonly used in music production and broadcasting.
     */
    namespace dynamic {
        class Limiter;
        class Compressor;
        class Expander;
        class Envelope;
    }
}