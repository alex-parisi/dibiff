/// Reverb.cpp

#include "Reverb.h"
#include "../inc/Eigen/Dense"
#include "../inc/imgui-knobs/imgui-knobs.h"

#include <iostream>

/**
 * @brief Constructor
 * @details Initializes the reverb object with a certain decay time
 * and room size
 * @param decayTime The decay time of the reverb in seconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 */
dibiff::effect::Reverb::Reverb(float decayTime, float roomSize, float sampleRate, int numDelays, float wetLevel) 
: dibiff::graph::AudioObject(), decayTime(decayTime), roomSize(roomSize), sampleRate(sampleRate), numDelays(numDelays), wetLevel(wetLevel) {
    name = "Reverb";
};
/**
 * @brief Initialize
 * @details Initializes the reverb connection points and delay buffers
 */
void dibiff::effect::Reverb::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(shared_from_this(), "ReverbInput"));
    output = std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "ReverbOutput"));
    // Initialize delay buffers
    int delayLength;
    for (int i = 0; i < numDelays; ++i) {
        delayLength = static_cast<int>((roomSize / speedOfSound) * sampleRate) * (i + 1);
        buffers.push_back(std::vector<float>(delayLength, 0.0f));
        bufferIndices.push_back(0);
    }
    feedback = std::pow(10, -3.0f * delayLength / (decayTime * sampleRate));
}
/**
 * @brief Process a sample
 * @details Processes a single sample of audio data
 * @param sample The input sample
 */
float dibiff::effect::Reverb::process(float sample) {
    float out = 0.0f;
    const float alpha = 0.5f; // Interpolation factor, adjust as needed
    for (int i = 0; i < numDelays; ++i) {
        int index = bufferIndices[i];
        int prevIndex = (index - 1 + buffers[i].size()) % buffers[i].size();
        float delayedSample = buffers[i][index];
        float prevDelayedSample = buffers[i][prevIndex];
        // Linear interpolation between the current and previous delayed samples
        float interpolatedSample = (1.0f - alpha) * prevDelayedSample + alpha * delayedSample;
        out += interpolatedSample;
        // Update the buffer with the new sample
        buffers[i][index] = sample + feedback * delayedSample;
        // Move to the next index
        bufferIndices[i] = (index + 1) % buffers[i].size();
    }
    return out / numDelays;
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::effect::Reverb::process() {
    /// Check if the decay time or room size has changed
    if (decayTime != lastDecayTime || roomSize != lastRoomSize) {
        /// If so, reset the delay buffers
        int delayLength;
        for (int i = 0; i < numDelays; ++i) {
            delayLength = static_cast<int>((roomSize / speedOfSound) * sampleRate) * (i + 1);
            buffers[i].resize(delayLength, 0.0f);
            bufferIndices[i] = 0;
        }
        feedback = std::pow(10, -3.0f * delayLength / (decayTime * sampleRate));
    }
    if (input->isReady()) {
        std::vector<float> audioData = *input->getData();
        int blockSize = input->getBlockSize();
        /// Insert audioData into the end of displaySamples
        displayInSamples.insert(displayInSamples.end(), audioData.begin(), audioData.end());
        Eigen::VectorXf x(blockSize), y(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            x(i) = audioData[i];
        }
        for (int i = 0; i < blockSize; ++i) {
            y(i) = process(x(i));
        }
        std::vector<float> out(blockSize);
        for (int i = 0; i < blockSize; ++i) {
            out[i] = wetLevel * y(i) + (1.0f - wetLevel) * x(i);
        }
        /// Insert out data into the end of displayOutSamples
        displayOutSamples.insert(displayOutSamples.end(), out.begin(), out.end());
        output->setData(out, blockSize);
        markProcessed();
    }
    lastDecayTime = decayTime;
    lastRoomSize = roomSize;
}
/**
 * @brief Reset the reverb
 * @details Resets the delay buffers
 */
void dibiff::effect::Reverb::reset() {
    for (int i = 0; i < numDelays; ++i) {
        bufferIndices[i] = 0;
        std::fill(buffers[i].begin(), buffers[i].end(), 0.0f);
    }
}
/**
 * @brief Clear the reverb
 * @details Clears the delay buffers
 */
void dibiff::effect::Reverb::clear() {
    for (int i = 0; i < numDelays; ++i) {
        std::fill(buffers[i].begin(), buffers[i].end(), 0.0f);
    }
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Reverb::getInput(int i) { return input; }
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Reverb::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::effect::Reverb::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the reverb is finished processing
 * @return True if the reverb is finished processing, false otherwise
 */
bool dibiff::effect::Reverb::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::effect::Reverb::isReadyToProcess() const {
    // Check if input is connected
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * Create a new reverb object
 * @param decayTime The decay time of the reverb in seconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 * @param numDelays The number of delay lines
 */
std::shared_ptr<dibiff::effect::Reverb> dibiff::effect::Reverb::create(float decayTime, float roomSize, float sampleRate, int numDelays, float wetLevel) {
    auto instance = std::make_shared<dibiff::effect::Reverb>(decayTime, roomSize, sampleRate, numDelays, wetLevel);
    instance->initialize();
    return instance;
}
/**
 * @brief Render the ImGui interface
 */
void dibiff::effect::Reverb::RenderImGui() {
    ImGui::SetNextWindowSize(ImVec2(344, 141), ImGuiCond_FirstUseEver);
    ImGui::Begin(getName().c_str());
    ImGuiKnobs::Knob("Decay\nTime", &decayTime, 0.01f, 10.0f, 0.01f, "%.01fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Room\nSize", &roomSize, 1.0f, 1000.0f, 1.0f, "%1.0fm", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Wet\nLevel", &wetLevel, 0.0f, 1.0f, 0.001f, "%.001f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGui::BeginChild("##ReverbElementPlots", ImVec2(0, 100), false);
    ImGui::PlotLines("Input", displayInSamples.data(), static_cast<int>(displayInSamples.size()), 0, NULL, -1.0f, 1.0f, ImVec2(100, 25));
    ImGui::PlotLines("Output", displayOutSamples.data(), static_cast<int>(displayOutSamples.size()), 0, NULL, -1.0f, 1.0f, ImVec2(100, 25));
    ImGui::EndChild();
    displayInSamples.clear();
    displayOutSamples.clear();
    ImGui::End();
}