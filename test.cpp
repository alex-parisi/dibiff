#include "dibiff"

int main() {

    float sampleRate = 48000.0f;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MidiInput
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create SineGenerator
    /// TODO: length and freq are not needed when controlled by MIDI
    auto sineGen = graph.add(dibiff::generator::SineGenerator::create(440.0f, sampleRate, -1, blockSize, 6));

    /// Create the WavWriter
    auto wavWriter = graph.add(dibiff::sink::WavWriter::create("midiOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sineGen->getInput());
    graph.connect(sineGen->getOutput(), audioPlayer->getInput());
    graph.connect(sineGen->getOutput(), wavWriter->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}