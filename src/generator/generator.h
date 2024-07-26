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
 * @brief Generator Voice
 * @details A generator voice object is a simple object that generates a signal
 * based on a certain function and parameters.
 */
class dibiff::generator::GeneratorVoice {
public:
    float frequency;
    bool active;
    int currentSample;
    int totalSamples;
    float sampleRate;
    int blockSize;

    GeneratorVoice() : frequency(0.0f), active(false), currentSample(0), totalSamples(-1), sampleRate(44100.0f), blockSize(512) {}

    void reset() {
        active = false;
        currentSample = 0;
    }
};
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
        std::vector<dibiff::generator::GeneratorVoice> voices;
        int numVoices;
        std::mutex voiceMutex;
        Generator(int numVoices = 1, float rate = 48000.0f, int blockSize = 512) 
        : dibiff::graph::AudioObject(), 
          frequency(0.0f), numVoices(numVoices), isActive(false) {
            voices.resize(numVoices);
            for (auto& voice : voices) {
                voice.sampleRate = rate;
                voice.blockSize = blockSize;
            }
        }
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
            unsigned char noteNumber = message[1];
            unsigned char velocity = message[2];
            float frequency = midiNoteToFrequency(noteNumber);

            if (type == 0x90 && velocity > 0) { // Note on
                std::lock_guard<std::mutex> lock(voiceMutex);
                int voiceIndex = findFreeVoice();
                if (voiceIndex >= 0) {
                    voices[voiceIndex].frequency = frequency;
                    voices[voiceIndex].active = true;
                    voices[voiceIndex].currentSample = 0;
                }
            } else if (type == 0x80 || (type == 0x90 && velocity == 0)) { // Note off
                std::lock_guard<std::mutex> lock(voiceMutex);
                for (auto& voice : voices) {
                    if (voice.frequency == frequency) {
                        voice.active = false;
                        // break;
                    }
                }
            }
        }
        /**
         * @brief Converts a MIDI note number to a frequency
         * @param noteNumber The MIDI note number
         */
        static float midiNoteToFrequency(int noteNumber) {
            return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
        }
        /**
         * @brief Finds a free voice
         */
        int findFreeVoice() {
            for (int i = 0; i < numVoices; ++i) {
                if (!voices[i].active) {
                    return i;
                }
            }
            return -1; // No free voice found
        }
        /**
         * @brief Determines if the generator is active
         */
        bool getIsActive() const { return isActive; }
        /**
         * @brief Gets the frequency of the generator
         */
        float getFrequency() const { return frequency; }
};