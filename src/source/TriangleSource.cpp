/// TriangleSource.cpp

#include "TriangleSource.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::source::TriangleSource::getName() const { return "TriangleSource"; }
/**
 * @brief Constructor
 * @details Initializes the triangle wave source with a certain frequency,
 * sample rate, total number of samples, and block size
 * @param level The gain of the triangle wave
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the triangle wave
 */
dibiff::source::TriangleSource::TriangleSource(float level, float freq, float rate, int samples, int blockSize)
: dibiff::graph::AudioCompositeObject(), 
    level(level), frequency(freq), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize) {};
/**
 * @brief Initialize
 * @details Initializes the triangle wave source connection points
 */
void dibiff::source::TriangleSource::initialize() {
    triangleGenerator = dibiff::generator::TriangleGenerator::create(frequency, sampleRate, totalSamples, blockSize);
    gain = dibiff::level::Gain::create(level);
    objects.push_back(triangleGenerator);
    objects.push_back(gain);
    dibiff::graph::AudioGraph::connect(triangleGenerator->getOutput(), gain->getInput());
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::source::TriangleSource::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::source::TriangleSource::getOutput() { return gain->getOutput(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::source::TriangleSource::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * Create a new triangle wave source object
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the triangle wave
 */
std::shared_ptr<dibiff::source::TriangleSource> dibiff::source::TriangleSource::create(float gain, float freq, float rate, int samples, int blockSize) {
    auto instance = std::make_shared<dibiff::source::TriangleSource>(gain, freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new triangle wave source object
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param duration The total duration of samples to generate
 * @param blockSize The block size of the triangle wave
 */
std::shared_ptr<dibiff::source::TriangleSource> dibiff::source::TriangleSource::create(float gain, float freq, float rate, std::chrono::duration<float> duration, int blockSize) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::source::TriangleSource>(gain, freq, rate, samples, blockSize);
    instance->initialize();
    return instance;
}
/**
 * Create a new triangle wave source object
 * @param freq The frequency of the triangle wave
 * @param rate The sample rate of the triangle wave
 * @param blockSize The block size of the triangle wave
 */
std::shared_ptr<dibiff::source::TriangleSource> dibiff::source::TriangleSource::create(float gain, float freq, float rate, int blockSize) {
    auto instance = std::make_shared<dibiff::source::TriangleSource>(gain, freq, rate, -1, blockSize);
    instance->initialize();
    return instance;
}