#include "../dibiff"

int main() {

    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    /// Create Baby's First Synth
    dibiff::synth::BabysFirstSynthParameters params;
    params.blockSize = blockSize;
    params.sampleRate = sampleRate;
    params.midiPortNum = 0;
    params.numVoices = 16;
    params.gain = 12.0f;
    params.attack = 0.1f;
    params.decay = 0.1f;
    params.sustain = 0.5f;
    params.release = 0.5f;
    params.modulationRate = 0.4f;
    params.modulationDepth = 2.0f;
    auto babysFirstSynth = graph.add(dibiff::synth::BabysFirstSynth::create(params));

    /// Create Reverb
    auto reverb = graph.add(dibiff::effect::Reverb::create(2, 10, sampleRate));
    reverb->setName("reverb");

    /// Create the WavWriter
    auto wavWriter1 = graph.add(dibiff::sink::WavWriter::create("synthOutput.wav", sampleRate));
    wavWriter1->setName("synth-output");
    auto wavWriter2 = graph.add(dibiff::sink::WavWriter::create("reverbOutput.wav", sampleRate));
    wavWriter2->setName("reverb-output");

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::GraphSink::create(sampleRate, blockSize));
    audioPlayer->setName("audio-player");

    /// Connect everything
    graph.connect(babysFirstSynth->getOutput(), reverb->getInput());
    graph.connect(babysFirstSynth->getOutput(), wavWriter1->getInput());
    graph.connect(reverb->getOutput(), wavWriter2->getInput());
    graph.connect(reverb->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}