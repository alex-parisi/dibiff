#include "../dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create Keyboard Simulator Input
    auto keyboardSimulator = graph.add(dibiff::midi::KeyboardSimulator::create(blockSize));

    /// Create Sine Generator
    auto sineGenerator = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));

    /// Create Envelope
    auto envelope = graph.add(dibiff::dynamic::Envelope::create(0.1f, 0.1f, 0.5f, 0.5f, sampleRate));

    /// Create Gain
    auto gain = graph.add(dibiff::level::Gain::create(-6.0f));

    /// Create the WavWriter
    auto wavWriter = graph.add(dibiff::sink::WavWriter::create("gainOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(keyboardSimulator->getOutput(), sineGenerator->getInput());
    graph.connect(keyboardSimulator->getOutput(), envelope->getInput(1));
    graph.connect(sineGenerator->getOutput(), envelope->getInput());
    graph.connect(envelope->getOutput(), gain->getInput());
    graph.connect(gain->getOutput(), wavWriter->getInput());
    graph.connect(gain->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}