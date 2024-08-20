/// BabysFirstSynth.cpp

#include "BabysFirstSynth.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::synth::BabysFirstSynth::getName() const { return "BabysFirstSynth"; }
/**
 * @brief Constructor
 * @details Initializes Baby's First Synth with given parameters
 */
dibiff::synth::BabysFirstSynth::BabysFirstSynth(BabysFirstSynthParameters params)
: dibiff::graph::AudioCompositeObject(), params(params) {
    /// Initialize vectors
    sineGenerators.resize(params.numVoices);
    envelopes.resize(params.numVoices);
};
/**
 * @brief Initialize
 * @details Initializes the sine wave source connection points
 */
void dibiff::synth::BabysFirstSynth::initialize() {
    /// Create the objects:
    midiInput = dibiff::midi::MidiInput::create(params.blockSize);
    voiceSelector = dibiff::midi::VoiceSelector::create(params.blockSize, params.numVoices);
    voiceSelector->setName("babys-first-synth-voice-selector");
    mixer = dibiff::level::Mixer::create(params.numVoices);
    mixer->setName("babys-first-synth-mixer");
    gain = dibiff::level::Gain::create(params.gain);
    gain->setName("babys-first-synth-gain");
    float _sampleRate = params.sampleRate;
    tremolo = dibiff::effect::Tremolo::create(params.modulationRate, params.modulationDepth, _sampleRate);
    tremolo->setName("babys-first-synth-tremolo");
    for (int i = 0; i < params.numVoices; i++) {
        sineGenerators[i] = dibiff::generator::SineGenerator::create(params.blockSize, params.sampleRate);
        sineGenerators[i]->setName("babys-first-synth-sine-generator");
        envelopes[i] = dibiff::dynamic::Envelope::create(params.attack, params.decay, params.sustain, params.release, _sampleRate);
        envelopes[i]->setName("babys-first-synth-envelope");
    }
    /// Add the objects to the graph
    objects.emplace_back(std::move(midiInput)); // 0
    objects.emplace_back(std::move(voiceSelector)); // 1
    objects.emplace_back(std::move(mixer)); // 2
    objects.emplace_back(std::move(gain)); // 3
    objects.emplace_back(std::move(tremolo)); // 4
    for (int i = 0; i < params.numVoices; i++) {
        objects.emplace_back(std::move(sineGenerators[i]));
        objects.emplace_back(std::move(envelopes[i]));
    }
    /// Connect everything
    dibiff::graph::AudioGraph::connect(objects[0]->getOutput(), objects[1]->getInput());
    int j = 0;
    for (int i = 0; i < params.numVoices; i++) {
        dibiff::graph::AudioGraph::connect(objects[1]->getOutput(i), objects[5 + j]->getInput());
        dibiff::graph::AudioGraph::connect(objects[1]->getOutput(i), objects[6 + j]->getInput(1));
        dibiff::graph::AudioGraph::connect(objects[5 + j]->getOutput(), objects[6 + j]->getInput());
        dibiff::graph::AudioGraph::connect(objects[6 + j]->getOutput(), objects[2]->getInput(i));
        ++++j;
    }
    dibiff::graph::AudioGraph::connect(objects[2]->getOutput(), objects[3]->getInput());
    dibiff::graph::AudioGraph::connect(objects[3]->getOutput(), objects[4]->getInput());
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
dibiff::graph::AudioConnectionPoint* dibiff::synth::BabysFirstSynth::getInput(int i) { return nullptr; };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
dibiff::graph::AudioConnectionPoint* dibiff::synth::BabysFirstSynth::getOutput(int i) { return objects[4]->getOutput(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
dibiff::graph::AudioConnectionPoint* dibiff::synth::BabysFirstSynth::getReference() { return nullptr; };
/**
 * Create a new sine wave source object
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
std::unique_ptr<dibiff::synth::BabysFirstSynth> dibiff::synth::BabysFirstSynth::create(dibiff::synth::BabysFirstSynthParameters params) {
    auto instance = std::make_unique<BabysFirstSynth>(params);
    instance->initialize();
    return std::move(instance);
}