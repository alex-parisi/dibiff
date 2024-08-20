/// Tremolo.cpp

#include "Tremolo.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Initializes the tremolo object with a certain modulation depth
 * and modulation rate
 * @param modulationDepth The modulation depth of the tremolo in milliseconds
 * @param modulationRate The modulation rate of the tremolo in Hz
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Tremolo::Tremolo(float& modulationDepth, float& modulationRate, float& sampleRate)
: dibiff::graph::AudioObject(), modulationDepth(modulationDepth), modulationRate(modulationRate), sampleRate(sampleRate) {
    name = "Tremolo";
};
/**
 * @brief Initialize
 * @details Initializes the tremolo connection points
 */
void dibiff::effect::Tremolo::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "TremoloInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "TremoloOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param input The input sample
 */
float dibiff::effect::Tremolo::process(float sample) {
    // Calculate the modulation factor
    float lfo = (1.0f + std::sin(2.0f * M_PI * modulationRate * phase)) / 2.0f;
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
    if (!input->isConnected()) {
        /// If no input is connected, just dump zeros into the output
        std::vector<float> out(input->getBlockSize(), 0.0f);
        output->setData(out, input->getBlockSize());
        markProcessed();
    } else if (input->isReady()) {
        const std::vector<float>& audioData = input->getData();
        const int blockSize = input->getBlockSize();
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = audioData[i];
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
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * Create a new tremolo object
 * @param modulationDepth The modulation depth of the tremolo in milliseconds
 * @param modulationRate The modulation rate of the tremolo in Hz
 * @param sampleRate The sample rate of the input signal
 */
std::unique_ptr<dibiff::effect::Tremolo> dibiff::effect::Tremolo::create(float& modulationDepth, float& modulationRate, float& sampleRate) {
    auto instance = std::make_unique<dibiff::effect::Tremolo>(modulationDepth, modulationRate, sampleRate);
    instance->initialize();
    return std::move(instance);
}