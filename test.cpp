#include "dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MidiInput
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create SineGenerators
    auto sineGen1 = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));
    auto sineGen2 = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));
    auto sineGen3 = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));

    /// Create Envelopes
    auto envelope1 = graph.add(dibiff::dynamic::Envelope::create(0.1, 0.1, 0.5, 1.0, sampleRate));
    auto envelope2 = graph.add(dibiff::dynamic::Envelope::create(0.1, 0.1, 0.5, 1.0, sampleRate));
    auto envelope3 = graph.add(dibiff::dynamic::Envelope::create(0.1, 0.1, 0.5, 1.0, sampleRate));

    /// Create VoiceSelector
    auto voiceSelector = graph.add(dibiff::midi::VoiceSelector::create(blockSize, 3));

    /// Create Mixer
    auto mixer = graph.add(dibiff::level::Mixer::create(3));

    /// Create Gain
    auto gain = graph.add(dibiff::level::Gain::create(-6.0f));

    /// Create Tremolo
    auto tremolo = graph.add(dibiff::effect::Tremolo::create(0.4, 2, sampleRate));

    /// Create the WavWriter
    auto wavWriter = graph.add(dibiff::sink::WavWriter::create("output.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    /// TODO: Connect vector of connections

    graph.connect(midiInput->getOutput(), voiceSelector->getInput());

    graph.connect(voiceSelector->getOutput(0), sineGen1->getInput());
    graph.connect(voiceSelector->getOutput(1), sineGen2->getInput());
    graph.connect(voiceSelector->getOutput(2), sineGen3->getInput());

    graph.connect(voiceSelector->getOutput(0), envelope1->getInput(1));
    graph.connect(voiceSelector->getOutput(1), envelope2->getInput(1));
    graph.connect(voiceSelector->getOutput(2), envelope3->getInput(1));

    graph.connect(sineGen1->getOutput(), envelope1->getInput());
    graph.connect(sineGen2->getOutput(), envelope2->getInput());
    graph.connect(sineGen3->getOutput(), envelope3->getInput());

    graph.connect(envelope1->getOutput(), mixer->getInput(0));
    graph.connect(envelope2->getOutput(), mixer->getInput(1));
    graph.connect(envelope3->getOutput(), mixer->getInput(2));

    graph.connect(mixer->getOutput(), gain->getInput());

    graph.connect(gain->getOutput(), tremolo->getInput());

    graph.connect(tremolo->getOutput(), wavWriter->getInput());
    graph.connect(tremolo->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}