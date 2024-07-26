/// SineGenerator.cpp

#include "SineGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::generator::SineGenerator::getName() const { return "SineGenerator"; }
/**
 * @brief Constructor
 * @details Initializes the sine wave source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
dibiff::generator::SineGenerator::SineGenerator(int blockSize, int sampleRate, float frequency, int totalSamples)
: dibiff::generator::Generator(), 
  blockSize(blockSize), sampleRate(sampleRate), frequency(frequency), totalSamples(totalSamples) {}
/**
 * @brief Initialize
 * @details Initializes the sine wave source connection points
 */
void dibiff::generator::SineGenerator::initialize() {
    input = std::make_shared<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(shared_from_this(), "SineGeneratorMidiInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "SineGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::SineGenerator::process() {
    /// If there is a duration set, and we've gone past it, stop generating samples
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    /// If the MIDI input is connected, process the MIDI messages to set the frequency
    float freq = frequency;
    if (input->isConnected() && input->isReady()) {
        auto midiData = *input->getData();
        for (const auto& message : midiData) {
            processMidiMessage(message);
        }
        freq = midiFrequency;
    }
    /// Generate Sine Wave samples at the specified frequency
    Eigen::VectorXf audioData(blockSize);
    audioData.setZero();
    Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(blockSize, currentSample, currentSample + blockSize - 1);
    audioData = (2.0f * M_PI * freq * indices / sampleRate).array().sin();
    currentSample += blockSize;
    /// Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    output->setData(out, out.size());
    markProcessed();
}
/**
 * @brief Reset the sine wave source
 * @details Resets the current sample index
 */
void dibiff::generator::SineGenerator::reset() {
    currentSample = 0;
    processed = false;
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SineGenerator::getInput(int i) { return input; };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SineGenerator::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SineGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::generator::SineGenerator::isReadyToProcess() const {
    if (totalSamples == -1) {
        return !processed;
    }
    return currentSample < totalSamples && !processed;
}
/**
 * @brief Check if the sine wave source is finished
 * @return True if the sine wave source has finished generating samples, false otherwise
 */
bool dibiff::generator::SineGenerator::isFinished() const {
    if (totalSamples == -1) {
        return false;
    }
    return currentSample >= totalSamples;
}
/**
 * Create a new sine wave source object
 * @param blockSize The block size of the sine wave
 * @param rate The sample rate of the sine wave
 * @param frequency The frequency of the sine wave, used if the MIDI input is not connected
 * @param totalSamples The total number of samples to generate
 */
std::shared_ptr<dibiff::generator::SineGenerator> dibiff::generator::SineGenerator::create(int blockSize, float sampleRate, float frequency, int totalSamples) {
    auto instance = std::make_shared<dibiff::generator::SineGenerator>(blockSize, sampleRate, frequency, totalSamples);
    instance->initialize();
    return instance;
}
/**
 * Create a new sine wave source object
 * @param blockSize The block size of the sine wave
 * @param rate The sample rate of the sine wave
 * @param frequency The frequency of the sine wave, used if the MIDI input is not connected
 * @param duration The duration of the sine wave
 */
std::shared_ptr<dibiff::generator::SineGenerator> dibiff::generator::SineGenerator::create(int blockSize, float sampleRate, float frequency, std::chrono::duration<int> duration) {
    int totalSamples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * sampleRate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::SineGenerator>(blockSize, sampleRate, frequency, totalSamples);
    instance->initialize();
    return instance;
}