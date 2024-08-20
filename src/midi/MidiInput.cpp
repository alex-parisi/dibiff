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
    auto o = std::make_unique<dibiff::graph::MidiOutput>(dibiff::graph::MidiOutput(this, "MidiInputMidiOutput"));
    _outputs.emplace_back(std::move(o));
    output = static_cast<dibiff::graph::MidiOutput*>(_outputs.back().get());
    midiRingBuffer = std::make_unique<RingBuffer<std::vector<unsigned char>>>(1024);
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of MIDI data
 */
void dibiff::midi::MidiInput::process() {
    std::vector<std::vector<unsigned char>> messages = {};
    while (midiRingBuffer->available() > 0) {
        std::vector<std::vector<unsigned char>> midiMessage = midiRingBuffer->read(midiRingBuffer->available());
        if (!midiMessage.empty()) {
            for (auto message : midiMessage) {
                messages.push_back(message);
            }
        }
    }
    /// Set the MIDI data to the output
    output->setData(messages, blockSize);
    markProcessed();
}
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
std::unique_ptr<dibiff::midi::MidiInput> dibiff::midi::MidiInput::create(int blockSize) {
    auto instance = std::make_unique<dibiff::midi::MidiInput>(blockSize);
    instance->initialize();
    return std::move(instance);
}
/**
 * @brief Add a MIDI message
 * @details Adds a MIDI message to the MIDI input
 * @param message The MIDI message to add
 */
void dibiff::midi::MidiInput::addMidiMessage(const std::vector<std::vector<unsigned char>> message) {
    midiRingBuffer->write(message, 1);
}