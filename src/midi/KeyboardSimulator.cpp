/// KeyboardSimulator.cpp

#include "KeyboardSimulator.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 */
std::string dibiff::midi::KeyboardSimulator::getName() const { return "KeyboardSimulator"; }
/**
 * @brief Constructor
 * @details Constructs a MIDI input object
 * @param blockSize The block size of the MIDI input
 */
dibiff::midi::KeyboardSimulator::KeyboardSimulator(int blockSize)
: dibiff::graph::AudioObject(), blockSize(blockSize) {}
/**
 * @brief Initialize
 * @details Initializes the MIDI input object
 */
void dibiff::midi::KeyboardSimulator::initialize() {
    output = std::make_shared<dibiff::graph::MidiOutput>(dibiff::graph::MidiOutput(shared_from_this(), "KeyboardSimulatorOutput"));
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of MIDI data
 */
void dibiff::midi::KeyboardSimulator::process() {
    std::vector<std::vector<unsigned char>> messages;
    messages = std::move(simulatedMidiEvents);
    simulatedMidiEvents.clear();
    // Set the MIDI data to the output
    output->setData(messages, blockSize);
    markProcessed();
}
/**
 * @brief Get the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::KeyboardSimulator::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); }
/**
 * @brief Get the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::KeyboardSimulator::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::KeyboardSimulator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); }
/**
 * @brief Check if the object is ready to process
 */
bool dibiff::midi::KeyboardSimulator::isReadyToProcess() const {
    return !processed;
}
/**
 * @brief Check if the object is finished
 */
bool dibiff::midi::KeyboardSimulator::isFinished() const {
    return false;
}
/**
 * @brief Render the ImGui interface
 */
void dibiff::midi::KeyboardSimulator::RenderImGui() {
    ImGui::Begin(getName().c_str());
    ImGui::Text("Settings for %s", getName().c_str());
    ImGuiIO& io = ImGui::GetIO();
    for (int key = ImGuiKey_Tab; key < ImGuiKey_COUNT; key++) {
        // Check if the key is pressed and was not already active
        if (ImGui::IsKeyPressed(ImGuiKey(key)) && activeKeys.find(key) == activeKeys.end()) {
            auto it = keyToMidiNote.find(key);
            if (it != keyToMidiNote.end()) {
                int midiNote = it->second;
                // Generate MIDI note on event
                std::cout << "MIDI Note On: " << midiNote << " (Key " << key << ")" << std::endl;
                // Record the key as active
                activeKeys.insert(key);
                /// Convert to MIDI message
                unsigned char message[3];
                message[0] = 0x90; // Note on
                message[1] = midiNote; // Note number
                message[2] = 127; // Velocity
                /// Add to vector - this is thread-safe
                simulatedMidiEvents.push_back(std::vector<unsigned char>(message, message + 3));
            }
        }
        // Check if the key is released and was active
        if (ImGui::IsKeyReleased(ImGuiKey(key)) && activeKeys.find(key) != activeKeys.end()) {
            auto it = keyToMidiNote.find(key);
            if (it != keyToMidiNote.end()) {
                int midiNote = it->second;
                // Generate MIDI note off event
                std::cout << "MIDI Note Off: " << midiNote << " (Key " << key << ")" << std::endl;
                // Remove the key from active keys
                activeKeys.erase(key);
                /// Convert to MIDI message
                unsigned char message[3];
                message[0] = 0x80; // Note off
                message[1] = midiNote; // Note number
                message[2] = 127; // Velocity
                /// Add to vector - this is thread-safe
                simulatedMidiEvents.push_back(std::vector<unsigned char>(message, message + 3));
            }
        }
    }
    ImGui::End();
}
/**
 * @brief Create a new MIDI input object
 * @param blockSize The block size of the MIDI input
 * @param portNum The port number of the MIDI input
 */
std::shared_ptr<dibiff::midi::KeyboardSimulator> dibiff::midi::KeyboardSimulator::create(int blockSize) {
    auto instance = std::make_shared<dibiff::midi::KeyboardSimulator>(blockSize);
    instance->initialize();
    return instance;
}