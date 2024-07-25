#include "dibiff"

int main() {

    float sampleRate = 48000.0f;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MidiInput
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create SineGenerator
    auto sineGen = graph.add(dibiff::generator::SineGenerator::create(440.0f, sampleRate, -1, blockSize));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sineGen->getInput());
    graph.connect(sineGen->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}