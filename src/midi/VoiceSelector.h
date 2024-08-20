/// VoiceSelector.h

#pragma once

#include "midi.h"
#include "../graph/graph.h"

class dibiff::midi::Voice {
    public:
        float frequency;
        bool active;
        std::vector<unsigned char> midiMessage;

        Voice() : frequency(1000.0f), active(false) {}

        void reset() {
            active = false;
        }
};

class dibiff::midi::VoiceSelector : public dibiff::graph::AudioObject {
    public:
        dibiff::graph::MidiInput* input;
        std::vector<dibiff::midi::Voice> voices;
        VoiceSelector(int blockSize, int numVoices = 3);
        void initialize() override;
        void process() override;
        void reset() override {};
        void clear() override {}
        bool isReadyToProcess() const override;
        bool isFinished() const override;
        static std::unique_ptr<VoiceSelector> create(int blockSize, int numVoices = 3);
    private:
        int blockSize;
        int numVoices;
        int voiceIndex = 0;
        void processMidiMessage(std::vector<unsigned char> message);
        float midiNoteToFrequency(int noteNumber);
};