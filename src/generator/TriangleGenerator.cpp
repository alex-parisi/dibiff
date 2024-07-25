/// TriangleGenerator.cpp

#include "TriangleGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::generator::TriangleGenerator::getName() const { return "TriangleGenerator"; }
/**
 * @brief Constructor
 * @details Initializes the triangle wave source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the triangle wave
 */
dibiff::generator::TriangleGenerator::TriangleGenerator(float freq, float rate, int samples, int blockSize)
: dibiff::generator::Generator(), 
    frequency(freq), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize) {};
/**
 * @brief Initialize
 * @details Initializes the triangle wave source connection points
 */
void dibiff::generator::TriangleGenerator::initialize() {
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "TriangleGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 * Unfortunately this requires double precision to prevent floating point errors 
 * contributing to phase misalignment.
 */
void dibiff::generator::TriangleGenerator::process() {
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    // Use double precision for sample rate and frequency
    const double periodSamples = static_cast<double>(sampleRate) / frequency;
    int effectiveBlockSize = (totalSamples == -1) ? blockSize : std::min(blockSize, totalSamples - currentSample);
    // Generate indices for the current block using double precision
    Eigen::VectorXd indices = Eigen::VectorXd::LinSpaced(effectiveBlockSize, static_cast<double>(currentSample), static_cast<double>(currentSample + effectiveBlockSize - 1));
    // Apply the 1/4 period offset
    Eigen::VectorXd offsetIndices = indices.array() + periodSamples / 4.0;
    // Calculate position in period for each sample
    Eigen::VectorXd positionInPeriod = offsetIndices.array() - (offsetIndices.array() / periodSamples).floor() * periodSamples;
    // Calculate the triangle wave values based on the position in the period
    Eigen::VectorXd triangleWave = (positionInPeriod.array() < periodSamples / 2.0).select(
        -1.0 + 2.0 * (positionInPeriod.array() / (periodSamples / 2.0)),
        1.0 - 2.0 * ((positionInPeriod.array() - periodSamples / 2.0) / (periodSamples / 2.0))
    );
    // Convert Eigen::VectorXd to std::vector<float>
    std::vector<float> audioData(triangleWave.data(), triangleWave.data() + triangleWave.size());
    currentSample += effectiveBlockSize;
    // Adjust the audio data size if exceeding total samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.resize(totalSamples - currentSample + blockSize);
    }
    // Set data to output and mark as processed
    output->setData(audioData, audioData.size());
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
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::TriangleGenerator::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::TriangleGenerator::getOutput() { return output; }
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
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the triangle wave
 */
std::shared_ptr<dibiff::generator::TriangleGenerator> dibiff::generator::TriangleGenerator::create(float freq, float rate, int samples, int blockSize) {
    auto instance = std::make_shared<dibiff::generator::TriangleGenerator>(freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new triangle wave source object
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param duration The total number of samples to generate
 * @param blockSize The block size of the triangle wave
 */
std::shared_ptr<dibiff::generator::TriangleGenerator> dibiff::generator::TriangleGenerator::create(float freq, float rate, std::chrono::duration<float> duration, int blockSize) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::TriangleGenerator>(freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}