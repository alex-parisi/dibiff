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
    midiInput = dibiff::midi::MidiInput::create(params.blockSize, params.midiPortNum);
    voiceSelector = dibiff::midi::VoiceSelector::create(params.blockSize, params.numVoices);
    mixer = dibiff::level::Mixer::create(params.numVoices);
    gain = dibiff::level::Gain::create(params.gain);
    tremolo = dibiff::effect::Tremolo::create(params.modulationRate, params.modulationDepth, params.sampleRate);
    for (int i = 0; i < params.numVoices; i++) {
        sineGenerators[i] = dibiff::generator::SineGenerator::create(params.blockSize, params.sampleRate);
        envelopes[i] = dibiff::dynamic::Envelope::create(params.attack, params.decay, params.sustain, params.release, params.sampleRate);
    }
    /// Add the objects to the graph
    objects.push_back(midiInput);
    objects.push_back(voiceSelector);
    for (int i = 0; i < params.numVoices; i++) {
        objects.push_back(sineGenerators[i]);
        objects.push_back(envelopes[i]);
    }
    objects.push_back(mixer);
    objects.push_back(gain);
    objects.push_back(tremolo);
    /// Connect everything
    dibiff::graph::AudioGraph::connect(midiInput->getOutput(), voiceSelector->getInput());
    for (int i = 0; i < params.numVoices; i++) {
        dibiff::graph::AudioGraph::connect(voiceSelector->getOutput(i), sineGenerators[i]->getInput());
        dibiff::graph::AudioGraph::connect(voiceSelector->getOutput(i), envelopes[i]->getInput(1));
        dibiff::graph::AudioGraph::connect(sineGenerators[i]->getOutput(), envelopes[i]->getInput());
        dibiff::graph::AudioGraph::connect(envelopes[i]->getOutput(), mixer->getInput(i));
    }
    dibiff::graph::AudioGraph::connect(mixer->getOutput(), gain->getInput());
    dibiff::graph::AudioGraph::connect(gain->getOutput(), tremolo->getInput());
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::synth::BabysFirstSynth::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::synth::BabysFirstSynth::getOutput(int i) { return tremolo->getOutput(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::synth::BabysFirstSynth::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * Create a new sine wave source object
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
std::shared_ptr<dibiff::synth::BabysFirstSynth> dibiff::synth::BabysFirstSynth::create(dibiff::synth::BabysFirstSynthParameters params) {
    auto instance = std::make_shared<BabysFirstSynth>(params);
    instance->initialize();
    return instance;
}