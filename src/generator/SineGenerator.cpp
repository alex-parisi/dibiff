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
dibiff::generator::SineGenerator::SineGenerator(float freq, float rate, int samples, int blockSize)
: dibiff::generator::Generator(), 
    frequency(freq), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize) {};
/**
 * @brief Initialize
 * @details Initializes the sine wave source connection points
 */
void dibiff::generator::SineGenerator::initialize() {
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "SineGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::SineGenerator::process() {
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(blockSize, currentSample, currentSample + blockSize - 1);
    Eigen::VectorXf audioData = (2.0f * M_PI * frequency * indices / sampleRate).array().sin();
    // Update currentSample to reflect the processed block
    currentSample += blockSize;
    // Check if the current sample exceeds the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    // Convert Eigen::VectorXf to std::vector<float>
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
std::weak_ptr<dibiff::graph::AudioInput> dibiff::generator::SineGenerator::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::generator::SineGenerator::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::generator::SineGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
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
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
std::shared_ptr<dibiff::generator::SineGenerator> dibiff::generator::SineGenerator::create(float freq, float rate, int samples, int blockSize) {
    auto instance = std::make_shared<dibiff::generator::SineGenerator>(freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new sine wave source object
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param duration The total duration of samples to generate
 * @param blockSize The block size of the sine wave
 */
std::shared_ptr<dibiff::generator::SineGenerator> dibiff::generator::SineGenerator::create(float freq, float rate, std::chrono::duration<float> duration, int blockSize) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::SineGenerator>(freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}