/// MidiInput.cpp

#include "MidiInput.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 */
std::string dibiff::midi::MidiInput::getName() const {
    return "MidiInput";
}
/**
 * @brief Constructor
 * @details Constructs a MIDI input object
 * @param blockSize The block size of the MIDI input
 * @param portNum The port number of the MIDI input
 */
dibiff::midi::MidiInput::MidiInput(int blockSize, int portNum)
: dibiff::graph::AudioObject(), blockSize(blockSize), portNum(portNum) {}
/**
 * @brief Initialize
 * @details Initializes the MIDI input object
 */
void dibiff::midi::MidiInput::initialize() {
    output = std::make_shared<dibiff::graph::MidiOutput>(dibiff::graph::MidiOutput(shared_from_this(), "MidiInputOutput"));
    midiIn = std::make_unique<RtMidiIn>();
    // Check for available ports.
    unsigned int nPorts = midiIn->getPortCount();
    std::cout << "\n[" << nPorts << "] MIDI input source(s) available." << std::endl;
    for (unsigned int i = 0; i < nPorts; i++) {
        std::string portName;
        try {
            portName = midiIn->getPortName(i);
        } catch (RtMidiError &error) {
            error.printMessage();
        }
        std::cout << "\tInput Port #" << i << ": " << portName << std::endl;
    }
    // Open the specified port.
    try {
        midiIn->openPort(portNum);
    } catch (RtMidiError &error) {
        throw error;
    }
    // Don't ignore sysex, timing, or active sensing messages.
    midiIn->ignoreTypes(false, false, false);
    // Set the callback function for MIDI input.
    midiIn->setCallback(&MidiInput::midiCallback, this);
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of MIDI data
 */
void dibiff::midi::MidiInput::process() {
    std::vector<std::vector<unsigned char>> messages;
    // Lock the mutex and move the events to the local variable
    {
        std::lock_guard<std::mutex> lock(midiMutex);
        messages = std::move(midiEvents);
        midiEvents.clear();
    }
    // Set the MIDI data to the output
    output->setData(messages, blockSize);
    markProcessed();
}
/**
 * @brief MIDI callback function
 * @details The callback function to handle incoming MIDI messages
 * @param deltatime The time since the last message
 * @param message The MIDI message
 * @param userData The user data
 */
void dibiff::midi::MidiInput::midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    auto *midiInput = static_cast<dibiff::midi::MidiInput *>(userData);
    if (message->size() > 0) {
        std::lock_guard<std::mutex> lock(midiInput->midiMutex);
        midiInput->midiEvents.push_back(*message);
    }
}
/**
 * @brief Get the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getInput(int i) {
    return std::weak_ptr<dibiff::graph::AudioInput>();
}
/**
 * @brief Get the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getOutput() {
    return output;
}
/**
 * @brief Get the reference connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getReference() {
    return std::weak_ptr<dibiff::graph::AudioReference>();
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
std::shared_ptr<dibiff::midi::MidiInput> dibiff::midi::MidiInput::create(int blockSize, int portNum) {
    auto instance = std::make_shared<dibiff::midi::MidiInput>(blockSize, portNum);
    instance->initialize();
    return instance;
}