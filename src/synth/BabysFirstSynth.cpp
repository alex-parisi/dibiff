/// BabysFirstSynth.cpp

#include "BabysFirstSynth.h"

void dibiff::synth::BabysFirstSynthGui::initialize() {
    name = "BabysFirstSynthGui";
};

void dibiff::synth::BabysFirstSynthGui::RenderImGui() {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    ImVec2 window_pos = ImVec2(50, 50);
    ImVec2 window_size = ImVec2(550, 350);

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_FirstUseEver);

    ImGui::Begin("Baby's First Synth", nullptr, window_flags);

    ImGuiID dockspace_id = ImGui::GetID("BabysFirstSynthDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    static bool first_time = true;
    if (first_time) {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, window_size);

        // Vertically split the dockspace into two nodes
        ImGuiID dock_id_voice_selector = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.35f, nullptr, &dockspace_id);
        ImGuiID dock_id_gain = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.5f, nullptr, &dockspace_id);

        // Dock the sub-windows into the stacked nodes
        ImGui::DockBuilderDockWindow("babys-first-synth-voice-selector", dock_id_voice_selector);
        ImGui::DockBuilderDockWindow("babys-first-synth-gain", dock_id_gain);
        ImGui::DockBuilderDockWindow("babys-first-synth-tremolo", dockspace_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImVec2 currentWindowSize = ImGui::GetWindowSize();
    std::cout << "Current window size: " << currentWindowSize.x << "x" << currentWindowSize.y << std::endl;
    ImGui::End(); // End the main parent window
}
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
    voiceSelector->setName("babys-first-synth-voice-selector");
    mixer = dibiff::level::Mixer::create(params.numVoices);
    mixer->setName("babys-first-synth-mixer");
    gain = dibiff::level::Gain::create(params.gain);
    gain->setName("babys-first-synth-gain");
    tremolo = dibiff::effect::Tremolo::create(params.modulationRate, params.modulationDepth, params.sampleRate);
    tremolo->setName("babys-first-synth-tremolo");
    for (int i = 0; i < params.numVoices; i++) {
        sineGenerators[i] = dibiff::generator::SineGenerator::create(params.blockSize, params.sampleRate);
        sineGenerators[i]->setName("babys-first-synth-sine-generator");
        envelopes[i] = dibiff::dynamic::Envelope::create(params.attack, params.decay, params.sustain, params.release, params.sampleRate);
        envelopes[i]->setName("babys-first-synth-envelope");
    }
    gui = std::make_shared<BabysFirstSynthGui>();
    gui->initialize();
    /// Add the objects to the graph
    objects.push_back(gui);
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