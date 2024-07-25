/// SquareSource.cpp

#include "SquareSource.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::source::SquareSource::getName() const { return "SquareSource"; }
/**
 * @brief Constructor
 * @details Initializes the square wave source with a certain frequency,
 * sample rate, total number of samples, block size, and duty cycle
 * @param level The gain of the square wave
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
dibiff::source::SquareSource::SquareSource(float level, float freq, float rate, int samples, int blockSize, float dutyCycle)
: dibiff::graph::AudioCompositeObject(), 
    level(level), frequency(freq), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize), dutyCycle(dutyCycle) {};
/**
 * @brief Initialize
 * @details Initializes the square wave source connection points
 */
void dibiff::source::SquareSource::initialize() {
    squareGenerator = dibiff::generator::SquareGenerator::create(frequency, sampleRate, totalSamples, blockSize, dutyCycle);
    gain = dibiff::level::Gain::create(level);
    objects.push_back(squareGenerator);
    objects.push_back(gain);
    dibiff::graph::AudioGraph::connect(squareGenerator->getOutput(), gain->getInput());
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::source::SquareSource::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::source::SquareSource::getOutput() { return gain->getOutput(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::source::SquareSource::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * Create a new square wave source object
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
std::shared_ptr<dibiff::source::SquareSource> dibiff::source::SquareSource::create(float gain, float freq, float rate, int samples, int blockSize, float dutyCycle) {
    auto instance = std::make_shared<dibiff::source::SquareSource>(gain, freq, rate, samples, blockSize, dutyCycle);
    instance->initialize();
    return instance;
}
/**
 * Create a new square wave source object
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param duration The total duration of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
std::shared_ptr<dibiff::source::SquareSource> dibiff::source::SquareSource::create(float gain, float freq, float rate, std::chrono::duration<float> duration, int blockSize, float dutyCycle) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::source::SquareSource>(gain, freq, rate, samples, blockSize, dutyCycle);
    instance->initialize();
    return instance;
}
/**
 * Create a new square wave source object
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
std::shared_ptr<dibiff::source::SquareSource> dibiff::source::SquareSource::create(float gain, float freq, float rate, int blockSize, float dutyCycle) {
    auto instance = std::make_shared<dibiff::source::SquareSource>(gain, freq, rate, -1, blockSize, dutyCycle);
    instance->initialize();
    return instance;
}