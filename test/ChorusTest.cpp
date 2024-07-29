#include "../dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MIDI Input
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create Sine Generator
    auto sineGenerator = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));

    /// Create Envelope
    auto envelope = graph.add(dibiff::dynamic::Envelope::create(0.1f, 0.1f, 0.5f, 0.5f, sampleRate));

    /// Create Gain
    auto gain = graph.add(dibiff::level::Gain::create(-6.0f));

    /// Create Chorus
    auto chorus = graph.add(dibiff::effect::Chorus::create(0.4f, 2.0f, sampleRate));

    /// Create the WavWriter
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("chorusOutput.wav", sampleRate));
    auto wavWriter2 = graph.add(dibiff::sink::WavWriter::create("gainOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sineGenerator->getInput());
    graph.connect(midiInput->getOutput(), envelope->getInput(1));
    graph.connect(sineGenerator->getOutput(), envelope->getInput());
    graph.connect(envelope->getOutput(), gain->getInput());
    graph.connect(gain->getOutput(), chorus->getInput());
    graph.connect(gain->getOutput(), wavWriter2->getInput());
    graph.connect(chorus->getOutput(), wavWriter1->getInput());
    graph.connect(chorus->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}