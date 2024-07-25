/// MidiInput.cpp

/// SineGenerator.cpp

#include "MidiInput.h"
#include "../inc/Eigen/Dense"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::midi::MidiInput::getName() const { return "MidiInput"; }
/**
 * @brief Constructor
 * @details Constructs a MIDI input object
 */
dibiff::midi::MidiInput::MidiInput(int blockSize, int portNum)
: dibiff::graph::AudioObject(), blockSize(blockSize), portNum(portNum) {};
/**
 * @brief Initialize
 * @details Initializes the MIDI input object
 */
void dibiff::midi::MidiInput::initialize() {
    output = std::make_shared<dibiff::graph::MidiOutput>(dibiff::graph::MidiOutput(shared_from_this(), "MidiInputOutput"));
    midiIn = std::make_unique<RtMidiIn>();
    // Check inputs.
    unsigned int nPorts = midiIn->getPortCount();
    std::cout << std::endl << "There are " << nPorts << " MIDI input sources available." << std::endl;
    std::string portName;
    for ( unsigned int i=0; i<nPorts; i++ ) {
        try {
            portName = midiIn->getPortName(i);
        } catch ( RtMidiError &error ) {
            error.printMessage();
        }
        std::cout << "\tInput Port #" << i << ": " << portName << std::endl << std::endl;
    }
    /// Open the port
    try {
        midiIn->openPort(portNum);
    } catch ( RtMidiError &error ) {
        error.printMessage();
    }
    /// Don't ignore sysex, timing, or active sensing messages.
    midiIn->ignoreTypes( false, false, false );
}
/**
 * @brief Generate a block of samples
 * @details Generates a block of audio data
 */
void dibiff::midi::MidiInput::process() {
    std::vector<unsigned char> message;
    std::vector<std::vector<unsigned char>> messages;
    while (midiIn->getMessage(&message) > 0) {
        messages.push_back(message);
    }
    output->setData(messages, blockSize);
    markProcessed();
}
/**
 * @brief Get the input connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getInput(int i) { return std::weak_ptr<dibiff::graph::AudioInput>(); };
/**
 * @brief Get the output connection point.
 * @return A shared pointer to the output connection point.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getOutput() { return output; }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::midi::MidiInput::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };
/**
 * @brief Check if the filter is ready to process
 * @return True if the filter is ready to process, false otherwise
 */
bool dibiff::midi::MidiInput::isReadyToProcess() const {
    return !processed;
}
/**
 * @brief Check if the MIDI input has finished generating samples
 * @return True if the MIDI input has finished generating samples, false otherwise
 */
bool dibiff::midi::MidiInput::isFinished() const {
    return false;
}
/**
 * Create a new MIDI input object
 */
std::shared_ptr<dibiff::midi::MidiInput> dibiff::midi::MidiInput::create(int blockSize, int portNum) {
    auto instance = std::make_shared<dibiff::midi::MidiInput>(blockSize, portNum);
    instance->initialize();
    return instance;
}