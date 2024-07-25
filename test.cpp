#include <iostream>
#include <memory>
#include <vector>
#include "dibiff"

int main() {

    float sampleRate = 48000.0f;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create sources
    auto sineSource = graph.add(dibiff::source::SineSource::create(-12.0f, 440.0f, sampleRate, std::chrono::seconds(10), blockSize));
    auto triangleSource = graph.add(dibiff::source::TriangleSource::create(-12.0f, 440.0f, sampleRate, std::chrono::seconds(10), blockSize));
    auto squareSource = graph.add(dibiff::source::SquareSource::create(-12.0f, 440.0f, sampleRate, std::chrono::seconds(10), blockSize));
    auto whiteNoiseSource = graph.add(dibiff::source::WhiteNoiseSource::create(-12.0f, sampleRate, std::chrono::seconds(10), blockSize));

    /// Create mixer with 4 inputs
    /// TODO: move Mixer to a different namespace, it's not really level
    auto mixer = graph.add(dibiff::level::Mixer::create(4));

    /// Create WavWriters
    /// TODO: Multi-input WAV writer
    auto wavSink1 = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));
    auto wavSink2 = graph.add(dibiff::sink::WavWriter::create("triangleOutput.wav", sampleRate));
    auto wavSink3 = graph.add(dibiff::sink::WavWriter::create("squareOutput.wav", sampleRate));
    auto wavSink4 = graph.add(dibiff::sink::WavWriter::create("whiteNoiseOutput.wav", sampleRate));
    auto wavSink5 = graph.add(dibiff::sink::WavWriter::create("mixerOutput.wav", sampleRate));

    /// Create MidiInput
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect sources to respective WavWriters
    graph.connect(sineSource->getOutput(), wavSink1->getInput());
    graph.connect(triangleSource->getOutput(), wavSink2->getInput());
    graph.connect(squareSource->getOutput(), wavSink3->getInput());
    graph.connect(whiteNoiseSource->getOutput(), wavSink4->getInput());

    /// Connect sources to mixer
    graph.connect(sineSource->getOutput(), mixer->getInput(0));
    graph.connect(triangleSource->getOutput(), mixer->getInput(1));
    graph.connect(squareSource->getOutput(), mixer->getInput(2));
    graph.connect(whiteNoiseSource->getOutput(), mixer->getInput(3));

    /// Connect mixer to WavWriter
    graph.connect(mixer->getOutput(), wavSink5->getInput());

    /// Connect mixer to AudioPlayer
    graph.connect(mixer->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}