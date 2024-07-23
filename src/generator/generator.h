/// generator.h

#pragma once

#include "../graph/graph.h"

namespace dibiff {
    /**
     * @brief Generator Namespace
     * @details The generator namespace contains a collection of simple audio
     * generator objects that can be used to generate audio signals.
     */
    namespace generator {
        class Generator;
        class SineGenerator;
        class TriangleGenerator;
        class SquareGenerator;
        class WhiteNoiseGenerator;
    }
}

/**
 * @brief Generic Generator class
 * @details A generic generator object is a simple object that generates a
 * signal based on a certain function and parameters.
 */
class dibiff::generator::Generator : public dibiff::graph::AudioObject {
    public:
        Generator() : dibiff::graph::AudioObject() {}
};