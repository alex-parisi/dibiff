#include <iostream>
#include <memory>
#include <vector>
#include "dibiff"

int main() {

    float sampleRate = 48000.0f;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    auto sineSource = graph.add(dibiff::source::SineSource::create(-6.0f, 5.0f, sampleRate, blockSize));
    auto wavSink1 = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));
    graph.connect(sineSource->getOutput(), wavSink1->getInput());

    auto triangleSource = graph.add(dibiff::source::TriangleSource::create(-6.0f, 5.0f, sampleRate, blockSize));
    auto wavSink2 = graph.add(dibiff::sink::WavWriter::create("triangleOutput.wav", sampleRate));
    graph.connect(triangleSource->getOutput(), wavSink2->getInput());

    auto squareSource = graph.add(dibiff::source::SquareSource::create(-6.0f, 5.0f, sampleRate, blockSize));
    auto wavSink3 = graph.add(dibiff::sink::WavWriter::create("squareOutput.wav", sampleRate));
    graph.connect(squareSource->getOutput(), wavSink3->getInput());

    auto whiteNoiseSource = graph.add(dibiff::source::WhiteNoiseSource::create(-6.0f, sampleRate, blockSize));
    auto wavSink4 = graph.add(dibiff::sink::WavWriter::create("whiteNoiseOutput.wav", sampleRate));
    graph.connect(whiteNoiseSource->getOutput(), wavSink4->getInput());
    
    graph.run(true, sampleRate, blockSize);

    return 0;
}