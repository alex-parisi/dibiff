/// TriangleGenerator.cpp

#include "TriangleGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the triangle wave source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param blockSize The block size of the triangle wave
 * @param sampleRate The sample rate of the triangle wave
 * @param frequency The frequency of the triangle wave
 * @param totalSamples The total number of samples to generate
 */
dibiff::generator::TriangleGenerator::TriangleGenerator(int blockSize, int sampleRate, float frequency, int totalSamples)
: dibiff::generator::Generator(), 
  blockSize(blockSize), sampleRate(sampleRate), frequency(frequency), totalSamples(totalSamples), phase(0.0f) {
    name = "TriangleGenerator";
}
/**
 * @brief Initialize
 * @details Initializes the triangle wave source connection points
 */
void dibiff::generator::TriangleGenerator::initialize() {
    input = std::make_shared<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(shared_from_this(), "SquareGeneratorMidiInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "TriangleGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 * Unfortunately this requires double precision to prevent floating point errors 
 * contributing to phase misalignment.
 */
void dibiff::generator::TriangleGenerator::process() {
    // If there is a duration set, and we've gone past it, stop generating samples
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    // If the MIDI input is connected, process the MIDI messages to set the frequency
    float freq = frequency;
    if (input->isConnected()) {
        auto midiData = *input->getData();
        for (const auto& message : midiData) {
            processMidiMessage(message);
        }
        freq = midiFrequency;
    }
    // Calculate phase increment based on the current frequency
    double phaseIncrement = 2.0 * M_PI * freq / static_cast<double>(sampleRate);
    // Generate samples using Eigen vectorized operations
    Eigen::VectorXd indices = Eigen::VectorXd::LinSpaced(blockSize, 0, blockSize - 1);
    Eigen::VectorXd phaseArray = (indices.array() * phaseIncrement + phase + M_PI / 2.0).cast<double>();
    // Wrap phase values within [0, 2π]
    phaseArray = phaseArray.unaryExpr([](double x) { return std::fmod(x, 2.0 * M_PI); });
    // Calculate the triangle wave values based on the phase
    Eigen::VectorXd audioData = phaseArray.unaryExpr([](double x) {
        return x < M_PI ? -1.0 + (2.0 / M_PI) * x : 3.0 - (2.0 / M_PI) * x;
    });
    // Update the current sample count and phase
    currentSample += blockSize;
    phase = std::fmod(phase + blockSize * phaseIncrement, 2.0 * M_PI);
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
 * @brief Reset the triangle wave source
 * @details Resets the current sample index
 */
void dibiff::generator::TriangleGenerator::reset() {
    currentSample = 0;
    processed = false;
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::TriangleGenerator::getInput(int i) { return input; };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::TriangleGenerator::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::TriangleGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::generator::TriangleGenerator::isReadyToProcess() const {
    if (totalSamples == -1) {
        return !processed;
    }
    return currentSample < totalSamples && !processed;
}
/**
 * @brief Check if the triangle wave source is finished
 * @return True if the triangle wave source has finished generating samples, false otherwise
 */
bool dibiff::generator::TriangleGenerator::isFinished() const {
    if (totalSamples == -1) {
        return false;
    }
    return currentSample >= totalSamples;
}
/**
 * Create a new triangle wave source object
 * @param blockSize The block size of the triangle wave
 * @param sampleRate The sample rate of the triangle wave
 * @param frequency The frequency of the triangle wave
 * @param totalSamples The total number of samples to generate
 */
std::shared_ptr<dibiff::generator::TriangleGenerator> dibiff::generator::TriangleGenerator::create(int blockSize, int sampleRate, float frequency, int totalSamples) {
    auto instance = std::make_shared<dibiff::generator::TriangleGenerator>(blockSize, sampleRate, frequency, totalSamples);
    instance->initialize();
    return instance;
}
/**
 * Create a new triangle wave source object
 * @param blockSize The block size of the triangle wave
 * @param sampleRate The sample rate of the triangle wave
 * @param frequency The frequency of the triangle wave
 * @param duration The total duration of samples to generate
 */
std::shared_ptr<dibiff::generator::TriangleGenerator> dibiff::generator::TriangleGenerator::create(int blockSize, int sampleRate, float frequency, std::chrono::duration<int> duration) {
    int totalSamples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * sampleRate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::TriangleGenerator>(blockSize, sampleRate, frequency, totalSamples);
    instance->initialize();
    return instance;
}