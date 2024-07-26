/// SquareGenerator.cpp

#include "SquareGenerator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::generator::SquareGenerator::getName() const { return "SquareGenerator"; }
/**
 * @brief Constructor
 * @details Initializes the square wave source with a certain frequency,
 * sample rate, total number of samples, block size, and duty cycle
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
dibiff::generator::SquareGenerator::SquareGenerator(float freq, float rate, int samples, int blockSize, int numVoices, float dutyCycle)
: dibiff::generator::Generator(numVoices, rate, blockSize), 
    frequency(freq), sampleRate(rate), currentSample(0), totalSamples(samples), blockSize(blockSize), dutyCycle(dutyCycle), previousActive(false) {};
/**
 * @brief Initialize
 * @details Initializes the square wave source connection points
 */
void dibiff::generator::SquareGenerator::initialize() {
    input = std::make_shared<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(shared_from_this(), "SquareGeneratorMidiInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "SquareGeneratorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::generator::SquareGenerator::process() {
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    if (input->isConnected() && input->isReady()) {
        auto midiData = *input->getData();
        for (const auto& message : midiData) {
            processMidiMessage(message);
        }
    }


    Eigen::VectorXf audioData(blockSize);
    audioData.setZero();
    int activeVoices = 0;
    {
        std::lock_guard<std::mutex> lock(voiceMutex);
        for (auto& voice : voices) {
            if (voice.active) {
                activeVoices++;
                // Generate the square wave if active
                // Calculate the period of the wave in samples
                const float period = voice.sampleRate / voice.frequency;
                int effectiveBlockSize = (voice.totalSamples == -1) ? blockSize : std::min(blockSize, voice.totalSamples - voice.currentSample);
                // Generate indices for the current block
                Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(effectiveBlockSize, voice.currentSample, voice.currentSample + effectiveBlockSize - 1);
                // Calculate phase for each sample using Eigen's array operations
                Eigen::VectorXf phases = (indices.array() / period).array() - ((indices.array() / period).array().floor());
                // Calculate the square wave values based on the phase and duty cycle
                Eigen::VectorXf voiceData = (phases.array() < dutyCycle).select(Eigen::VectorXf::Constant(effectiveBlockSize, 1.0f), Eigen::VectorXf::Constant(effectiveBlockSize, -1.0f));
                audioData += voiceData;
                voice.currentSample += blockSize;

                if (voice.totalSamples != -1 && voice.currentSample >= voice.totalSamples) {
                    voice.active = false;
                }
            }
        }
    }
    audioData /= activeVoices;
    // Convert Eigen::VectorXf to std::vector<float> and set the output buffer
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    output->setData(out, out.size());
    markProcessed();
}
/**
 * @brief Reset the square wave source
 * @details Resets the current sample index
 */
void dibiff::generator::SquareGenerator::reset() {
    currentSample = 0;
    processed = false;
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SquareGenerator::getInput(int i) { return input; };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SquareGenerator::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SquareGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::generator::SquareGenerator::isReadyToProcess() const {
    if (totalSamples == -1) {
        return !processed;
    }
    return currentSample < totalSamples && !processed;
}
/**
 * @brief Check if the square wave source is finished
 * @return True if the square wave source has finished generating samples, false otherwise
 */
bool dibiff::generator::SquareGenerator::isFinished() const {
    if (totalSamples == -1) {
        return false;
    }
    return currentSample >= totalSamples;
}
/**
 * Create a new square wave source object
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param samples The total number of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
std::shared_ptr<dibiff::generator::SquareGenerator> dibiff::generator::SquareGenerator::create(float freq, float rate, int samples, int blockSize, int numVoices, float dutyCycle) {
    auto instance = std::make_shared<dibiff::generator::SquareGenerator>(freq, rate, samples, blockSize, numVoices, dutyCycle);
    instance->initialize();
    return instance;
}
/**
 * Create a new square wave source object
 * @param freq The frequency of the square wave
 * @param rate The sample rate of the square wave
 * @param duration The total number of samples to generate
 * @param blockSize The block size of the square wave
 * @param dutyCycle The duty cycle of the square wave (default is 0.5)
 */
std::shared_ptr<dibiff::generator::SquareGenerator> dibiff::generator::SquareGenerator::create(float freq, float rate, std::chrono::duration<float> duration, int blockSize, int numVoices, float dutyCycle) {
    int samples = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * rate / 1000.0f);
    auto instance = std::make_shared<dibiff::generator::SquareGenerator>(freq, rate, samples, blockSize, numVoices, dutyCycle);
    instance->initialize();
    return instance;
}