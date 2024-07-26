/// generator.h

#pragma once

#include "../graph/graph.h"

#include <iostream>

namespace dibiff {
    /**
     * @brief Generator Namespace
     * @details The generator namespace contains a collection of simple audio
     * generator objects that can be used to generate audio signals.
     */
    namespace generator {
        class GeneratorVoice;
        class Generator;
        class SineGenerator;
        class TriangleGenerator;
        class SquareGenerator;
        class WhiteNoiseGenerator;
    }
}

/// TODO: Move these to a different file...

/**
 * @brief Generic Generator class
 * @details A generic generator object is a simple object that generates a
 * signal based on a certain function and parameters.
 */
class dibiff::generator::Generator : public dibiff::graph::AudioObject {
    protected:
        float midiFrequency;
    public:
        Generator() 
        : dibiff::graph::AudioObject(), 
          midiFrequency(0.0f) {}
        /**
         * @brief Processes a MIDI message
         * @param message The MIDI message to process
         */
        void processMidiMessage(std::vector<unsigned char> message) {
            if (message.empty()) return;

            unsigned char status = message[0];
            unsigned char type = status & 0xF0;
            unsigned char noteNumber = message[1];
            unsigned char velocity = message[2];
            float freq = midiNoteToFrequency(noteNumber);

            if (type == 0x90 && velocity > 0) { // Note on
                midiFrequency = freq;
            }
        }
        /**
         * @brief Converts a MIDI note number to a frequency
         * @param noteNumber The MIDI note number
         */
        static float midiNoteToFrequency(int noteNumber) {
            return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
        }
};