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

    /// Create Envelope
    auto envelope = graph.add(dibiff::dynamic::Envelope::create(0.1f, 0.5f, 0.8f, 1.0f, sampleRate));

    /// Create a Gain block
    auto gain = graph.add(dibiff::level::Gain::create(-6.0f));

    /// Create a Tremolo effect
    /// TODO: FIX TREMOLO
    auto tremolo = graph.add(dibiff::effect::Tremolo::create(0.4, 2, sampleRate));

    /// Create a Reverb effect
    /// TODO: FIX REVERB
    auto reverb = graph.add(dibiff::effect::Reverb::create(2, 10, sampleRate));

    /// Create the WavWriters
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("sineOutput.wav", sampleRate));
    auto wavWriter2 = graph.add(dibiff::sink::WavWriter::create("envelopeOutput.wav", sampleRate));
    auto wavWriter3 = graph.add(dibiff::sink::WavWriter::create("gainOutput.wav", sampleRate));
    auto wavWriter4 = graph.add(dibiff::sink::WavWriter::create("tremoloOutput.wav", sampleRate));
    auto wavWriter5 = graph.add(dibiff::sink::WavWriter::create("reverbOutput.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(midiInput->getOutput(), sineGen->getInput());
    graph.connect(midiInput->getOutput(), envelope->getInput(1));
    graph.connect(sineGen->getOutput(), envelope->getInput());
    graph.connect(sineGen->getOutput(), wavWriter1->getInput());
    graph.connect(envelope->getOutput(), gain->getInput());
    graph.connect(envelope->getOutput(), wavWriter2->getInput());
    graph.connect(gain->getOutput(), tremolo->getInput());
    graph.connect(gain->getOutput(), wavWriter3->getInput());
    graph.connect(tremolo->getOutput(), reverb->getInput());
    graph.connect(tremolo->getOutput(), wavWriter4->getInput());
    graph.connect(reverb->getOutput(), wavWriter5->getInput());
    graph.connect(reverb->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}