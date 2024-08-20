/// VariableGenerator.cpp

#include "VariableGenerator.h"
#include "../inc/Eigen/Dense"

dibiff::generator::VariableGenerator::VariableGenerator(int blockSize, int sampleRate, float& state, float dutyCycle, float frequency, int totalSamples)
: dibiff::generator::Generator(), 
  blockSize(blockSize), sampleRate(sampleRate), dutyCycle(dutyCycle), frequency(frequency), totalSamples(totalSamples), state(state), phase(0.0f) {
    name = "VariableGenerator";
}

void dibiff::generator::VariableGenerator::initialize() {
    auto i = std::make_unique<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(this, "VariableGeneratorMidiInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::MidiInput*>(_inputs.back().get());
    auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "VariableGeneratorOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::AudioOutput*>(_outputs.back().get());
}

void dibiff::generator::VariableGenerator::process() {
    // If there is a duration set, and we've gone past it, stop generating samples
    if (totalSamples != -1 && currentSample >= totalSamples) {
        return;
    }
    // If the MIDI input is connected, process the MIDI messages to set the frequency
    float freq = frequency;
    if (input->isConnected()) {
        const auto& midiData = input->getData();
        for (const auto& message : midiData) {
            processMidiMessage(message);
        }
        freq = midiFrequency;
    }
    GeneratorState _state = (GeneratorState)state;
    switch (_state) {
        case GeneratorState::Sine:
            generateSine(freq);
            break;
        case GeneratorState::Square:
            generateSquare(freq);
            break;
        case GeneratorState::Triangle:
            generateTriangle(freq);
            break;
        default:
            generateSine(freq);
            break;
    }
}

void dibiff::generator::VariableGenerator::generateSine(float freq) {
    // Calculate phase increment based on the current frequency
    float phaseIncrement = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    // Generate samples using Eigen vectorized operations
    Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(blockSize, 0, blockSize - 1);
    Eigen::VectorXf phaseArray = (indices.array() * phaseIncrement + phase).cast<float>();
    // Wrap phase values within [0, 2π]
    phaseArray = phaseArray.unaryExpr([](float x) { return std::fmod(x, 2.0f * static_cast<float>(M_PI)); });
    Eigen::VectorXf audioData = phaseArray.array().sin();
    // Update the current sample count and phase
    currentSample += blockSize;
    phase = std::fmod(phase + blockSize * phaseIncrement, 2.0f * static_cast<float>(M_PI));
    // Update the last frequency to the new frequency
    lastFrequency = freq;
    // Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    // Output the generated audio data
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    output->setData(out, out.size());
    markProcessed();
}

void dibiff::generator::VariableGenerator::generateSquare(float freq) {
    // Calculate phase increment based on the current frequency
    float phaseIncrement = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    // Generate samples using Eigen vectorized operations
    Eigen::VectorXf indices = Eigen::VectorXf::LinSpaced(blockSize, 0, blockSize - 1);
    Eigen::VectorXf phaseArray = (indices.array() * phaseIncrement + phase).cast<float>();
    // Wrap phase values within [0, 2π]
    phaseArray = phaseArray.unaryExpr([](float x) { return std::fmod(x, 2.0f * static_cast<float>(M_PI)); });
    // Calculate the square wave values based on the phase and duty cycle
    Eigen::VectorXf audioData = (phaseArray.array() < dutyCycle * 2.0f * static_cast<float>(M_PI))
        .select(Eigen::VectorXf::Constant(blockSize, 1.0f), Eigen::VectorXf::Constant(blockSize, -1.0f));
    // Update the current sample count and phase
    currentSample += blockSize;
    phase = std::fmod(phase + blockSize * phaseIncrement, 2.0f * static_cast<float>(M_PI));
    // Update the last frequency to the new frequency
    lastFrequency = freq;
    // Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    // Output the generated audio data
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    output->setData(out, out.size());
    markProcessed();
}

void dibiff::generator::VariableGenerator::generateTriangle(float freq) {
    // Calculate phase increment based on the current frequency
    double phaseIncrement = 2.0 * M_PI * freq / static_cast<double>(sampleRate);
    // Generate samples using Eigen vectorized operations
    Eigen::VectorXd indices = Eigen::VectorXd::LinSpaced(blockSize, 0, blockSize - 1);
    Eigen::VectorXd phaseArray = (indices.array() * phaseIncrement + phase + M_PI / 2.0).cast<double>();
    // Wrap phase values within [0, 2π]
    phaseArray = phaseArray.unaryExpr([](double x) { return std::fmod(x, 2.0 * M_PI); });
    // Calculate the triangle wave values based on the phase
    Eigen::VectorXd audioData = phaseArray.unaryExpr([](double x) {
        return x < M_PI ? -1.0 + (2.0 / M_PI) * x : 3.0 - (2.0 / M_PI) * x;
    });
    // Update the current sample count and phase
    currentSample += blockSize;
    phase = std::fmod(phase + blockSize * phaseIncrement, 2.0 * M_PI);
    // Update the last frequency to the new frequency
    lastFrequency = freq;
    // Preserve size if we've exceeded the total number of samples
    if (totalSamples != -1 && currentSample > totalSamples) {
        audioData.conservativeResize(totalSamples - currentSample + blockSize);
    }
    // Output the generated audio data
    std::vector<float> out(audioData.data(), audioData.data() + audioData.size());
    output->setData(out, out.size());
    markProcessed();
}

void dibiff::generator::VariableGenerator::reset() {
    currentSample = 0;
    processed = false;
}

bool dibiff::generator::VariableGenerator::isReadyToProcess() const {
    if (totalSamples == -1) {
        return !processed;
    }
    return currentSample < totalSamples && !processed;
}

bool dibiff::generator::VariableGenerator::isFinished() const {
    if (totalSamples == -1) {
        return false;
    }
    return currentSample >= totalSamples;
}

std::unique_ptr<dibiff::generator::VariableGenerator> dibiff::generator::VariableGenerator::create(int blockSize, float sampleRate, float& state, float dutyCycle, float frequency, int totalSamples) {
    auto instance = std::make_unique<dibiff::generator::VariableGenerator>(blockSize, sampleRate, state, dutyCycle, frequency, totalSamples);
    instance->initialize();
    return std::move(instance);
}