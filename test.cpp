#include <iostream>
#include <memory>
#include <vector>
#include "dibiff"

int main() {

    float sampleRate = 48000.0f;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    auto sineSource = graph.add(dibiff::source::SineSource::create(-12.0f, 440.0f, sampleRate, std::chrono::seconds(5), blockSize));
    auto wavSink1 = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));
    graph.connect(sineSource->getOutput(), wavSink1->getInput());

    auto triangleSource = graph.add(dibiff::source::TriangleSource::create(-12.0f, 440.0f, sampleRate, std::chrono::seconds(5), blockSize));
    auto wavSink2 = graph.add(dibiff::sink::WavWriter::create("triangleOutput.wav", sampleRate));
    graph.connect(triangleSource->getOutput(), wavSink2->getInput());

    auto squareSource = graph.add(dibiff::source::SquareSource::create(-12.0f, 440.0f, sampleRate, std::chrono::seconds(5), blockSize));
    auto wavSink3 = graph.add(dibiff::sink::WavWriter::create("squareOutput.wav", sampleRate));
    graph.connect(squareSource->getOutput(), wavSink3->getInput());

    auto whiteNoiseSource = graph.add(dibiff::source::WhiteNoiseSource::create(-12.0f, sampleRate, std::chrono::seconds(5), blockSize));
    auto wavSink4 = graph.add(dibiff::sink::WavWriter::create("whiteNoiseOutput.wav", sampleRate));
    graph.connect(whiteNoiseSource->getOutput(), wavSink4->getInput());

    auto mixer = graph.add(dibiff::level::Mixer::create(4));
    auto wavSink5 = graph.add(dibiff::sink::WavWriter::create("mixerOutput.wav", sampleRate));
    graph.connect(sineSource->getOutput(), mixer->getInput(0));
    graph.connect(triangleSource->getOutput(), mixer->getInput(1));
    graph.connect(squareSource->getOutput(), mixer->getInput(2));
    graph.connect(whiteNoiseSource->getOutput(), mixer->getInput(3));
    graph.connect(mixer->getOutput(), wavSink5->getInput());

    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));
    graph.connect(mixer->getOutput(), audioPlayer->getInput());

    graph.run(true);

    return 0;
}