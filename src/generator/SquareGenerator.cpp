/// SquareGenerator.cpp

#include "SquareGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the square wave source with a certain frequency,
 * sample rate, total number of samples, block size, and duty cycle
 * @param blockSize The block size of the square wave
 * @param sampleRate The sample rate of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 * @param frequency The frequency of the square wave, used if the MIDI input is not connected
 * @param totalSamples The total number of samples to generate
 */
dibiff::generator::SquareGenerator::SquareGenerator(int blockSize, int sampleRate, float dutyCycle, float frequency, int totalSamples)
: dibiff::generator::Generator(), 
  blockSize(blockSize), sampleRate(sampleRate), dutyCycle(dutyCycle), frequency(frequency), totalSamples(totalSamples), phase(0.0f) {
    name = "SquareGenerator";
}
/**
 * @brief Initialize
 * @details Initializes the square wave source connection points
 */
void dibiff::generator::SquareGenerator::initialize() {
    auto i = std::make_unique<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(this, "SquareGeneratorMidiInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::MidiInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "SquareGeneratorOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::SquareGenerator::process() {
    // If there is a duration set, and we've gone past it, stop generating samples
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    // If the MIDI input is connected, process the MIDI messages to set the frequency
    float freq = frequency;
    if (input->isConnected()) {
        const auto& midiData = input->getData();
        for (const auto& message : midiData) {
            processMidiMessage(message);
        }
        freq = midiFrequency;
    }
    // Calculate phase increment based on the current frequency
    float phaseIncrement = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    // Generate samples using Eigen vectorized operations
    Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(blockSize, 0, blockSize - 1);
    Eigen::VectorXf phaseArray = (indices.array() * phaseIncrement + phase).cast<float>();
    // Wrap phase values within [0, 2π]
    phaseArray = phaseArray.unaryExpr([](float x) { return std::fmod(x, 2.0f * static_cast<float>(M_PI)); });
    // Calculate the square wave values based on the phase and duty cycle
    Eigen::VectorXf audioData = (phaseArray.array() < dutyCycle * 2.0f * static_cast<float>(M_PI))
        .select(Eigen::VectorXf::Constant(blockSize, 1.0f), Eigen::VectorXf::Constant(blockSize, -1.0f));
    // Update the current sample count and phase
    currentSample += blockSize;
    phase = std::fmod(phase + blockSize * phaseIncrement, 2.0f * static_cast<float>(M_PI));
    // Update the last frequency to the new frequency
    lastFrequency = freq;
    // Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    // Output the generated audio data
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    output->setData(out, out.size());
    markProcessed();
}
/**
 * @brief Reset the square wave source
 * @details Resets the current sample index
 */
void dibiff::generator::SquareGenerator::reset() {
    currentSample = 0;
    processed = false;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::generator::SquareGenerator::isReadyToProcess() const {
    if (totalSamples == -1) {
        return !processed;
    }
    return currentSample < totalSamples && !processed;
}
/**
 * @brief Check if the square wave source is finished
 * @return True if the square wave source has finished generating samples, false otherwise
 */
bool dibiff::generator::SquareGenerator::isFinished() const {
    if (totalSamples == -1) {
        return false;
    }
    return currentSample >= totalSamples;
}
/**
 * Create a new square wave source object
 * @param blockSize The block size of the square wave
 * @param sampleRate The sample rate of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 * @param frequency The frequency of the square wave, used if the MIDI input is not connected
 * @param totalSamples The total number of samples to generate
 */
std::unique_ptr<dibiff::generator::SquareGenerator> dibiff::generator::SquareGenerator::create(int blockSize, int sampleRate, float dutyCycle, float frequency, int totalSamples) {
    auto instance = std::make_unique<dibiff::generator::SquareGenerator>(blockSize, sampleRate, dutyCycle, frequency, totalSamples);
    instance->initialize();
    return std::move(instance);
}
/**
 * Create a new square wave source object
 * @param blockSize The block size of the square wave
 * @param sampleRate The sample rate of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 * @param frequency The frequency of the square wave, used if the MIDI input is not connected
 * @param duration The length of time to generate samples
 */
std::unique_ptr<dibiff::generator::SquareGenerator> dibiff::generator::SquareGenerator::create(int blockSize, int sampleRate, float dutyCycle, float frequency, std::chrono::duration<int> duration) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * sampleRate / 1000.0f);
    auto instance = std::make_unique<dibiff::generator::SquareGenerator>(blockSize, sampleRate, dutyCycle, frequency, samples);
    instance->initialize();
    return std::move(instance);
}