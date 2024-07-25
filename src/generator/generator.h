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
        class Generator;
        class SineGenerator;
        class TriangleGenerator;
        class SquareGenerator;
        class WhiteNoiseGenerator;
    }
}

/// TODO: Move this to a different file...
/**
 * @brief Generic Generator class
 * @details A generic generator object is a simple object that generates a
 * signal based on a certain function and parameters.
 */
class dibiff::generator::Generator : public dibiff::graph::AudioObject {
    protected:
        float frequency;
        bool isActive;
    public:
        Generator() : dibiff::graph::AudioObject(), frequency(0.0f), isActive(false) {}
        /**
         * @brief Gets the name of a MIDI note
         * @param noteNumber The MIDI note number
         */
        std::string getNoteName(int noteNumber) {
            static const std::string noteNames[] = {
                "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
            };
            int octave = noteNumber / 12 - 1;
            int noteIndex = noteNumber % 12;
            return noteNames[noteIndex] + std::to_string(octave);
        }
        /**
         * @brief Processes a MIDI message
         * @param message The MIDI message to process
         */
        void processMidiMessage(std::vector<unsigned char> message) {
            if (message.empty()) return;

            unsigned char status = message[0];
            unsigned char type = status & 0xF0;
            unsigned char channel = status & 0x0F;

            if (type == 0x90 || type == 0x80) { // Note-on or Note-off
                unsigned char noteNumber = message[1];
                unsigned char velocity = message[2];
                if (type == 0x90 && velocity > 0) {
                    // Note On: Calculate frequency and activate the generator
                    frequency = midiNoteToFrequency(noteNumber);
                    isActive = true;
                } else {
                    // Note Off: Deactivate the generator
                    isActive = false;
                }
            } else {
            }
        }

        static float midiNoteToFrequency(int noteNumber) {
            return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
        }

        bool getIsActive() const { return isActive; }
        float getFrequency() const { return frequency; }
};