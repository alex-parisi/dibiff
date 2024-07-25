/// SineSource.cpp

#include "SineSource.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::source::SineSource::getName() const { return "SineSource"; }
/**
 * @brief Constructor
 * @details Initializes the sine wave source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param level The gain of the sine wave
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
dibiff::source::SineSource::SineSource(float level, float freq, float rate, int samples, int blockSize)
: dibiff::graph::AudioCompositeObject(), 
    level(level), frequency(freq), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize) {};
/**
 * @brief Initialize
 * @details Initializes the sine wave source connection points
 */
void dibiff::source::SineSource::initialize() {
    sineGenerator = dibiff::generator::SineGenerator::create(frequency, sampleRate, totalSamples, blockSize);
    gain = dibiff::level::Gain::create(level);
    objects.push_back(sineGenerator);
    objects.push_back(gain);
    dibiff::graph::AudioGraph::connect(sineGenerator->getOutput(), gain->getInput());
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::source::SineSource::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::source::SineSource::getOutput() { return gain->getOutput(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::source::SineSource::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * Create a new sine wave source object
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the sine wave
 */
std::shared_ptr<dibiff::source::SineSource> dibiff::source::SineSource::create(float gain, float freq, float rate, int samples, int blockSize) {
    auto instance = std::make_shared<SineSource>(gain, freq, rate, samples, blockSize);
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
std::shared_ptr<dibiff::source::SineSource> dibiff::source::SineSource::create(float gain, float freq, float rate, std::chrono::duration<float> duration, int blockSize) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::source::SineSource>(gain, freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new sine wave source object
 * @param freq The frequency of the sine wave
 * @param rate The sample rate of the sine wave
 * @param blockSize The block size of the sine wave
 */
std::shared_ptr<dibiff::source::SineSource> dibiff::source::SineSource::create(float gain, float freq, float rate, int blockSize) {
    auto instance = std::make_shared<dibiff::source::SineSource>(gain, freq, rate, -1, blockSize);
    instance->initialize();
    return instance;
}