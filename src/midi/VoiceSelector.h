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
    std::shared_ptr<dibiff::graph::MidiInput> input;
    std::vector<std::shared_ptr<dibiff::graph::MidiOutput>> outputs;
    std::vector<dibiff::midi::Voice> voices;
    public:
        VoiceSelector(int blockSize, int numVoices = 3);
        void initialize() override;
        void process() override;
        void reset() override {};
        void clear() override {}
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) override;
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) override;
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getReference() override;
        bool isReadyToProcess() const override;
        bool isFinished() const override;
        static std::shared_ptr<VoiceSelector> create(int blockSize, int numVoices = 3);
    private:
        int blockSize;
        int voiceIndex = 0;
        void processMidiMessage(std::vector<unsigned char> message);
        float midiNoteToFrequency(int noteNumber);
};