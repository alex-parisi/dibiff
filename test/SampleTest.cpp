#include "../dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create MIDI Input
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize, 0));

    /// Create Sample Generator
    auto sampleGenerator = graph.add(dibiff::generator::SampleGenerator::create("../test/testAudio/sample.wav", blockSize, sampleRate));

    /// Create Reverb
    auto reverb = graph.add(dibiff::effect::Reverb::create(2, 10, sampleRate));

    /// Create the WavWriter
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("sampleOutput.wav", sampleRate));
    auto wavWriter2 = graph.add(dibiff::sink::WavWriter::create("reverbOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sampleGenerator->getInput());
    graph.connect(sampleGenerator->getOutput(), wavWriter1->getInput());
    graph.connect(sampleGenerator->getOutput(), reverb->getInput());
    graph.connect(reverb->getOutput(), wavWriter2->getInput());
    graph.connect(reverb->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}