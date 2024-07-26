/// Tremolo.cpp

#include "Tremolo.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::effect::Tremolo::getName() const { return "Tremolo"; }
/**
 * @brief Constructor
 * @details Initializes the tremolo object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the tremolo (0 to 1)
 * @param modulationRate The modulation rate of the tremolo in Hz
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Tremolo::Tremolo(float modulationDepth, float modulationRate, float sampleRate)
: dibiff::graph::AudioObject(), 
    modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate) {};
/**
 * @brief Initialize
 * @details Initializes the tremolo connection points
 */
void dibiff::effect::Tremolo::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "TremoloInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "TremoloOutput"));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 */
float dibiff::effect::Tremolo::process(float sample) {
    // Calculate the modulation factor
    float lfo = (1.0f + std::sinf(2.0f * M_PI * modulationRate * phase)) / 2.0f;
    float modulation = (1.0f - modulationDepth) + modulationDepth * lfo;
    // Apply the modulation to the input signal
    float output = sample * modulation;
    // Increment the phase for the LFO
    phase += 1.0f / sampleRate;
    if (phase >= 1.0f) {
        phase -= 1.0f;
    }
    return output;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Tremolo::process() {
    if (input->isReady()) {
        std::vector<float> data = *input->getData();
        int blockSize = input->getBlockSize();
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = data[i];
        }
        for (int i = 0; i < blockSize; ++i) {
            y(i) = process(x(i));
        }
        std::vector<float> out(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            out[i] = y(i);
        }
        output->setData(out, blockSize);
        markProcessed();
    }
}
/**
 * @brief Reset the tremolo
 * @details Resets the phase of the LFO
 */
void dibiff::effect::Tremolo::reset() {
    phase = 0.0f;
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Tremolo::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Tremolo::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Tremolo::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the tremolo is finished processing
 * @return True if the tremolo is finished processing, false otherwise
 */
bool dibiff::effect::Tremolo::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Tremolo::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new tremolo object
 * @param modulationDepth The modulation depth of the tremolo in milliseconds
 * @param modulationRate The modulation rate of the tremolo in Hz
 * @param sampleRate The sample rate of the input signal
 */
std::shared_ptr<dibiff::effect::Tremolo> dibiff::effect::Tremolo::create(float modulationDepth, float modulationRate, float sampleRate) {
    auto instance = std::make_shared<dibiff::effect::Tremolo>(modulationDepth, modulationRate, sampleRate);
    instance->initialize();
    return instance;
}