/// MidiInput.cpp

#include "MidiInput.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Constructor
 * @details Constructs a MIDI input object
 * @param blockSize The block size of the MIDI input
 * @param portNum The port number of the MIDI input
 */
dibiff::midi::MidiInput::MidiInput(int blockSize)
: dibiff::graph::AudioObject(), blockSize(blockSize) {
    name = "MidiInput";
}
/**
 * @brief Initialize
 * @details Initializes the MIDI input object
 */
void dibiff::midi::MidiInput::initialize() {
    output = std::make_shared<dibiff::graph::MidiOutput>(dibiff::graph::MidiOutput(shared_from_this(), "MidiInputOutput"));
    midiRingBuffer = std::make_shared<RingBuffer<std::vector<unsigned char>>>(1024);
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of MIDI data
 */
void dibiff::midi::MidiInput::process() {
    std::vector<std::vector<unsigned char>> messages;
    std::vector<unsigned char> midiMessage;
    while (midiRingBuffer->available() > 0) {
        midiMessage.clear();
        size_t messagesRead = midiRingBuffer->read(&midiMessage, 1);
        if (messagesRead > 0) {
            messages.push_back(midiMessage);
        }
    }
    /// Set the MIDI data to the output
    output->setData(messages, blockSize);
    markProcessed();
}
/**
 * @brief Get the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); }
/**
 * @brief Get the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getOutput(int i) { return output; }
/**
 * @brief Get the reference connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); }
/**
 * @brief Check if the object is ready to process
 */
bool dibiff::midi::MidiInput::isReadyToProcess() const {
    return !processed;
}
/**
 * @brief Check if the object is finished
 */
bool dibiff::midi::MidiInput::isFinished() const {
    return false;
}
/**
 * @brief Create a new MIDI input object
 * @param blockSize The block size of the MIDI input
 * @param portNum The port number of the MIDI input
 */
std::shared_ptr<dibiff::midi::MidiInput> dibiff::midi::MidiInput::create(int blockSize) {
    auto instance = std::make_shared<dibiff::midi::MidiInput>(blockSize);
    instance->initialize();
    return instance;
}
/**
 * @brief Add a MIDI message
 * @details Adds a MIDI message to the MIDI input
 * @param message The MIDI message to add
 */
void dibiff::midi::MidiInput::addMidiMessage(const std::vector<unsigned char>& message) {
    midiRingBuffer->write(&message, 1);
}