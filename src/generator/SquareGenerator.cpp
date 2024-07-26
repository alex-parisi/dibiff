/// SquareGenerator.cpp

#include "SquareGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::generator::SquareGenerator::getName() const { return "SquareGenerator"; }
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
  blockSize(blockSize), sampleRate(sampleRate), dutyCycle(dutyCycle), frequency(frequency), totalSamples(totalSamples) {}
/**
 * @brief Initialize
 * @details Initializes the square wave source connection points
 */
void dibiff::generator::SquareGenerator::initialize() {
    input = std::make_shared<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(shared_from_this(), "SquareGeneratorMidiInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "SquareGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::SquareGenerator::process() {
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
    /// Generate Square Wave samples at the specified frequency
    Eigen::VectorXf audioData(blockSize);
    audioData.setZero();
    // Calculate the period of the wave in samples
    const float period = sampleRate / freq;
    int effectiveBlockSize = (totalSamples == -1) ? blockSize : std::min(blockSize, totalSamples - currentSample);
    // Generate indices for the current block
    Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(effectiveBlockSize, currentSample, currentSample + effectiveBlockSize - 1);
    // Calculate phase for each sample using Eigen's array operations
    Eigen::VectorXf phases = (indices.array() / period).array() - ((indices.array() / period).array().floor());
    // Calculate the square wave values based on the phase and duty cycle
    audioData = (phases.array() < dutyCycle).select(Eigen::VectorXf::Constant(effectiveBlockSize, 1.0f), Eigen::VectorXf::Constant(effectiveBlockSize, -1.0f));
    currentSample += effectiveBlockSize;
    /// Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
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
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SquareGenerator::getInput(int i) { return input; };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SquareGenerator::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SquareGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
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
std::shared_ptr<dibiff::generator::SquareGenerator> dibiff::generator::SquareGenerator::create(int blockSize, int sampleRate, float dutyCycle, float frequency, int totalSamples) {
    auto instance = std::make_shared<dibiff::generator::SquareGenerator>(blockSize, sampleRate, dutyCycle, frequency, totalSamples);
    instance->initialize();
    return instance;
}
/**
 * Create a new square wave source object
 * @param blockSize The block size of the square wave
 * @param sampleRate The sample rate of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 * @param frequency The frequency of the square wave, used if the MIDI input is not connected
 * @param duration The length of time to generate samples
 */
std::shared_ptr<dibiff::generator::SquareGenerator> dibiff::generator::SquareGenerator::create(int blockSize, int sampleRate, float dutyCycle, float frequency, std::chrono::duration<int> duration) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * sampleRate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::SquareGenerator>(blockSize, sampleRate, dutyCycle, frequency, samples);
    instance->initialize();
    return instance;
}