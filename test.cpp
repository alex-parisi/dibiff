#include "dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MidiInput
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create SineGenerator
    auto sineGen = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));

    /// Create the WavWriters
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sineGen->getInput());
    graph.connect(sineGen->getOutput(), wavWriter1->getInput());
    graph.connect(sineGen->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}