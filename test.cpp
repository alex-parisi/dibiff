#include <iostream>
#include <memory>
#include <vector>
#include "dibiff"

int main() {

    float sampleRate = 48000.0f;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    auto sineSource = graph.add(dibiff::source::SineSource::create(-12.0f, 440.0f, sampleRate, blockSize));
    auto wavSink = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));
    graph.connect(sineSource->getOutput(), wavSink->getInput());
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));
    graph.connect(sineSource->getOutput(), audioPlayer->getInput());

    // auto triangleSource = graph.add(dibiff::source::TriangleSource::create(-12.0f, 440.0f, sampleRate, blockSize));
    // auto wavSink = graph.add(dibiff::sink::WavWriter::create("triangleOutput.wav", sampleRate));
    // graph.connect(triangleSource->getOutput(), wavSink->getInput());
    // auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));
    // graph.connect(triangleSource->getOutput(), audioPlayer->getInput());

    // auto squareSource = graph.add(dibiff::source::SquareSource::create(-30.0f, 440.0f, sampleRate, blockSize));
    // auto wavSink3 = graph.add(dibiff::sink::WavWriter::create("squareOutput.wav", sampleRate));
    // graph.connect(squareSource->getOutput(), wavSink3->getInput());
    // auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));
    // graph.connect(squareSource->getOutput(), audioPlayer->getInput());

    // auto whiteNoiseSource = graph.add(dibiff::source::WhiteNoiseSource::create(-12.0f, sampleRate, blockSize));
    // auto wavSink = graph.add(dibiff::sink::WavWriter::create("whiteNoiseOutput.wav", sampleRate));
    // graph.connect(whiteNoiseSource->getOutput(), wavSink->getInput());
    // auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));
    // graph.connect(whiteNoiseSource->getOutput(), audioPlayer->getInput());

    graph.run(true);

    return 0;
}