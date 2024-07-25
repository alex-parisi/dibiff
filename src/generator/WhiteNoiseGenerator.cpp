/// WhiteNoiseGenerator.cpp

#include "WhiteNoiseGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::generator::WhiteNoiseGenerator::getName() const { return "WhiteNoiseGenerator"; }
/**
 * @brief Constructor
 * @details Initializes the white noise source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param freq The frequency of the white noise
 * @param rate The sample rate of the white noise
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the white noise
 */
dibiff::generator::WhiteNoiseGenerator::WhiteNoiseGenerator(float rate, int samples, int blockSize)
: dibiff::generator::Generator(), 
    engine(rd()), distribution(-1.0f, 1.0f),
    sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize) {};
/**
 * @brief Initialize
 * @details Initializes the white noise source connection points
 */
void dibiff::generator::WhiteNoiseGenerator::initialize() {
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "WhiteNoiseGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::WhiteNoiseGenerator::process() {
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    int effectiveBlockSize = (totalSamples == -1) ? blockSize : std::min(blockSize, totalSamples - currentSample);
    // Generate random numbers using Eigen's random number generation capabilities
    Eigen::VectorXf audioData = Eigen::VectorXf::NullaryExpr(effectiveBlockSize, [&]() { return distribution(engine); });
    // Convert Eigen::VectorXf to std::vector<float>
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    currentSample += effectiveBlockSize;
    // Set data to output and mark as processed
    output->setData(out, out.size());
    markProcessed();
}
/**
 * @brief Reset the white noise source
 * @details Resets the current sample index
 */
void dibiff::generator::WhiteNoiseGenerator::reset() {
    currentSample = 0;
    processed = false;
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::generator::WhiteNoiseGenerator::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::generator::WhiteNoiseGenerator::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::generator::WhiteNoiseGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::generator::WhiteNoiseGenerator::isReadyToProcess() const {
    if (totalSamples == -1) {
        return !processed;
    }
    return currentSample < totalSamples && !processed;
}
/**
 * @brief Check if the white noise source is finished
 * @return True if the white noise source has finished generating samples, false otherwise
 */
bool dibiff::generator::WhiteNoiseGenerator::isFinished() const {
    if (totalSamples == -1) {
        return false;
    }
    return currentSample >= totalSamples;
}
/**
 * Create a new white noise source object
 * @param freq The frequency of the white noise
 * @param rate The sample rate of the white noise
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the white noise
 */
std::shared_ptr<dibiff::generator::WhiteNoiseGenerator> dibiff::generator::WhiteNoiseGenerator::create(float rate, int samples, int blockSize) {
    auto instance = std::make_shared<dibiff::generator::WhiteNoiseGenerator>(rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new white noise source object
 * @param freq The frequency of the white noise
 * @param rate The sample rate of the white noise
 * @param duration The total duration of samples to generate
 * @param blockSize The block size of the white noise
 */
std::shared_ptr<dibiff::generator::WhiteNoiseGenerator> dibiff::generator::WhiteNoiseGenerator::create(float rate, std::chrono::duration<float> duration, int blockSize) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::WhiteNoiseGenerator>(rate, samples, blockSize);
    instance->initialize();
    return instance;
}