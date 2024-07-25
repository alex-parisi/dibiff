/// WhiteNoiseSource.cpp

#include "WhiteNoiseSource.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::source::WhiteNoiseSource::getName() const { return "WhiteNoiseSource"; }
/**
 * @brief Constructor
 * @details Initializes the white noise source with a certain level, sample rate,
 * total number of samples, and block size
 * @param level The gain of the white noise
 * @param rate The sample rate of the white noise
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the white noise
 */
dibiff::source::WhiteNoiseSource::WhiteNoiseSource(float level, float rate, int samples, int blockSize)
: dibiff::graph::AudioCompositeObject(), 
    level(level), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize) {};
/**
 * @brief Initialize
 * @details Initializes the white noise source connection points
 */
void dibiff::source::WhiteNoiseSource::initialize() {
    whiteNoiseGenerator = dibiff::generator::WhiteNoiseGenerator::create(sampleRate, totalSamples, blockSize);
    gain = dibiff::level::Gain::create(level);
    objects.push_back(whiteNoiseGenerator);
    objects.push_back(gain);
    dibiff::graph::AudioGraph::connect(whiteNoiseGenerator->getOutput(), gain->getInput());
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::source::WhiteNoiseSource::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::source::WhiteNoiseSource::getOutput() { return gain->getOutput(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::source::WhiteNoiseSource::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * Create a new white noise source object
 * @param rate The sample rate of the white noise
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the white noise
 */
std::shared_ptr<dibiff::source::WhiteNoiseSource> dibiff::source::WhiteNoiseSource::create(float gain, float rate, int samples, int blockSize) {
    auto instance = std::make_shared<dibiff::source::WhiteNoiseSource>(gain, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new white noise source object
 * @param rate The sample rate of the white noise
 * @param duration The total duration of samples to generate
 * @param blockSize The block size of the white noise
 */
std::shared_ptr<dibiff::source::WhiteNoiseSource> dibiff::source::WhiteNoiseSource::create(float gain, float rate, std::chrono::duration<float> duration, int blockSize) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::source::WhiteNoiseSource>(gain, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new white noise source object
 * @param rate The sample rate of the white noise
 * @param blockSize The block size of the white noise
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the white noise
 */
std::shared_ptr<dibiff::source::WhiteNoiseSource> dibiff::source::WhiteNoiseSource::create(float gain, float rate, int blockSize) {
    auto instance = std::make_shared<dibiff::source::WhiteNoiseSource>(gain, rate, -1, blockSize);
    instance->initialize();
    return instance;
}