/// WhiteNoiseGenerator.cpp

#include "WhiteNoiseGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the white noise source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param blockSize The block size of the white noise
 * @param sampleRate The sample rate of the white noise
 * @param totalSamples The total number of samples to generate
 */
dibiff::generator::WhiteNoiseGenerator::WhiteNoiseGenerator(int blockSize, int sampleRate, int totalSamples)
: dibiff::generator::Generator(), engine(rd()), distribution(-1.0f, 1.0f),
  blockSize(blockSize), sampleRate(sampleRate), totalSamples(totalSamples) {
    name = "WhiteNoiseGenerator";
}
/**
 * @brief Initialize
 * @details Initializes the white noise source connection points
 */
void dibiff::generator::WhiteNoiseGenerator::initialize() {
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "WhiteNoiseGeneratorOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::WhiteNoiseGenerator::process() {
    /// Generate White Noise samples
    Eigen::VectorXf audioData(blockSize);
    audioData.setZero();
    int effectiveBlockSize = (totalSamples == -1) ? blockSize : std::min(blockSize, totalSamples - currentSample);
    // Generate random numbers using Eigen's random number generation capabilities
    audioData = Eigen::VectorXf::NullaryExpr(effectiveBlockSize, [&]() { return distribution(engine); });
    currentSample += effectiveBlockSize;
    /// Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    // Convert Eigen::VectorXf to std::vector<float> and set the output buffer
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
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
 * @param blockSize The block size of the white noise
 * @param sampleRate The sample rate of the white noise
 * @param totalSamples The total number of samples to generate
 */
std::unique_ptr<dibiff::generator::WhiteNoiseGenerator> dibiff::generator::WhiteNoiseGenerator::create(int blockSize, int sampleRate, int totalSamples) {
    auto instance = std::make_unique<dibiff::generator::WhiteNoiseGenerator>(blockSize, sampleRate, totalSamples);
    instance->initialize();
    return std::move(instance);
}
/**
 * Create a new white noise source object
 * @param blockSize The block size of the white noise
 * @param sampleRate The sample rate of the white noise
 * @param duration The total duration of samples to generate
 */
std::unique_ptr<dibiff::generator::WhiteNoiseGenerator> dibiff::generator::WhiteNoiseGenerator::create(int blockSize, int sampleRate, std::chrono::duration<int> duration) {
    int totalSamples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * sampleRate / 1000.0f);
    auto instance = std::make_unique<dibiff::generator::WhiteNoiseGenerator>(blockSize, sampleRate, totalSamples);
    instance->initialize();
    return std::move(instance);
}