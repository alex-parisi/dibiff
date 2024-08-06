#include "../dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MIDI Input
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize));
    midiInput->setName("midi-input");

    /// Create Sine Generator
    auto sineGenerator = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));
    sineGenerator->setName("sine-generator");

    /// Create Envelope
    auto envelope = graph.add(dibiff::dynamic::Envelope::create(0.1f, 0.1f, 0.5f, 0.5f, sampleRate));
    envelope->setName("envelope");

    /// Create Gain
    float _gain = -6.0f;
    auto gain = graph.add(dibiff::level::Gain::create(_gain));
    gain->setName("gain");

    /// Create Chorus
    auto chorus = graph.add(dibiff::effect::Chorus::create(0.4f, 2.0f, sampleRate));
    chorus->setName("chorus");

    /// Create the WavWriter
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("chorusOutput.wav", sampleRate));
    wavWriter1->setName("chorus-output");
    auto wavWriter2 = graph.add(dibiff::sink::WavWriter::create("gainOutput.wav", sampleRate));
    wavWriter2->setName("gain-output");

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::GraphSink::create(1, sampleRate, blockSize));
    audioPlayer->setName("audio-player");

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