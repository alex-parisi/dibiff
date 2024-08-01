/// BabysFirstSynth.h

#pragma once

#include "synth.h"
#include "../graph/graph.h"

#include "../midi/MidiInput.h"
#include "../midi/VoiceSelector.h"
#include "../generator/SineGenerator.h"
#include "../dynamic/Envelope.h"
#include "../level/Mixer.h"
#include "../level/Gain.h"
#include "../effect/Tremolo.h"

/**
 * @brief BabysFirstSynthParameters
 * @details A struct that contains the parameters for Baby's First Synth
 */
struct dibiff::synth::BabysFirstSynthParameters {
    int blockSize = 0;
    int sampleRate = 0;
    int midiPortNum = 0;
    int numVoices = 0;
    float gain = 0.0f;
    float attack = 0.0f;
    float decay = 0.0f;
    float sustain = 0.0f;
    float release = 0.0f;
    float modulationRate = 0.0f;
    float modulationDepth = 0.0f;
};
/**
 * @brief Baby's First Synth GUI
 */
class dibiff::synth::BabysFirstSynthGui : public dibiff::graph::GuiObject {
    public:
        void initialize() override;
        void RenderImGui() override;
};
/**
 * @brief Baby's First Synth
 */
class dibiff::synth::BabysFirstSynth : public dibiff::graph::AudioCompositeObject {
    std::shared_ptr<dibiff::graph::AudioObject> midiInput;
    std::shared_ptr<dibiff::midi::VoiceSelector> voiceSelector;
    std::vector<std::shared_ptr<dibiff::generator::SineGenerator>> sineGenerators;
    std::vector<std::shared_ptr<dibiff::dynamic::Envelope>> envelopes;
    std::shared_ptr<dibiff::level::Mixer> mixer;
    std::shared_ptr<dibiff::level::Gain> gain;
    std::shared_ptr<dibiff::effect::Tremolo> tremolo;
    std::shared_ptr<dibiff::synth::BabysFirstSynthGui> gui;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes Baby's First Synth with given parameters
         */
        BabysFirstSynth(dibiff::synth::BabysFirstSynthParameters params);
        /**
         * @brief Initialize
         * @details Initializes Baby's First Synth's connection points
         */
        void initialize() override;
        /**
         * @brief Get the input connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) override;
        /**
         * @brief Get the output connection point.
         * @return A shared pointer to the output connection point.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) override;
        /**
         * @brief Get the reference connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getReference() override;
        /**
         * Create a new Baby's First Synth object
         * @param params The parameters for Baby's First Synth
         */
        static std::shared_ptr<BabysFirstSynth> create(dibiff::synth::BabysFirstSynthParameters params);
    private:
        dibiff::synth::BabysFirstSynthParameters params;
};