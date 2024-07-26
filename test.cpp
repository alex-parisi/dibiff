#include "dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MidiInput
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create SineGenerator
    auto sineGen = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));

    /// Create Envelope
    auto envelope = graph.add(dibiff::dynamic::Envelope::create(0.1, 0.1, 0.5, 1.0, sampleRate));

    /// Create the WavWriters
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));
    auto wavWriter2 = graph.add(dibiff::sink::WavWriter::create("envelopeOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sineGen->getInput());
    graph.connect(midiInput->getOutput(), envelope->getInput(1));
    graph.connect(sineGen->getOutput(), wavWriter1->getInput());
    graph.connect(sineGen->getOutput(), envelope->getInput());
    graph.connect(envelope->getOutput(), wavWriter2->getInput());
    graph.connect(envelope->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}