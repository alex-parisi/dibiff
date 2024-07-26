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
    params.numVoices = 3;
    params.gain = -6.0f;
    params.attack = 0.1f;
    params.decay = 0.1f;
    params.sustain = 0.5f;
    params.release = 1.0f;
    params.modulationRate = 0.4f;
    params.modulationDepth = 2.0f;
    auto babysFirstSynth = graph.add(dibiff::synth::BabysFirstSynth::create(params));

    /// Create the WavWriter
    auto wavWriter = graph.add(dibiff::sink::WavWriter::create("output.wav", sampleRate));

    /// Create AudioPlayer
    /// TODO: AudioPlayer must be added to the graph last - WHY?!
    auto audioPlayer = graph.add(dibiff::sink::AudioPlayer::create(sampleRate, blockSize));

    /// Connect everything
    graph.connect(babysFirstSynth->getOutput(), wavWriter->getInput());
    graph.connect(babysFirstSynth->getOutput(), audioPlayer->getInput());

    /// Run the graph
    graph.run(true);

    return 0;
}