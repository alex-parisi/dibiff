/// VoiceSelector.cpp

#include "VoiceSelector.h"
#include "../inc/Eigen/Dense"

#include <iostream>

/**
 * @brief Constructor
 * @param blockSize The block size of the object
 * @param numVoices The number of voices to create
 */
dibiff::midi::VoiceSelector::VoiceSelector(int blockSize, int numVoices)
: dibiff::graph::AudioObject(), blockSize(blockSize), numVoices(numVoices) {
    name = "VoiceSelector";
    for (int i = 0; i < numVoices; ++i) {
        voices.push_back(dibiff::midi::Voice());
    }
}
/**
 * @brief Initialize the object
 * @details Create the input and output connection points
 */
void dibiff::midi::VoiceSelector::initialize() {
    input = std::make_shared<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(shared_from_this(), "VoiceSelectorMidiInput"));
    _inputs.push_back(input);
    for (int i = 0; i < voices.size(); ++i) {
        _outputs.push_back(std::make_shared<dibiff::graph::MidiOutput>(dibiff::graph::MidiOutput(shared_from_this(), "VoiceSelectorMidiOutput" + std::to_string(i))));
    }
}
/**
 * @brief Process the object
 * @details Process the MIDI input and assign voices to the output
 */
void dibiff::midi::VoiceSelector::process() {
    /// First, clear all voice MIDI messages
    for (int i = 0; i < voices.size(); ++i) {
        voices[i].midiMessage.clear();
    }
    if (!input->isConnected()) {
        /// Assign empty MIDI messages to all voices
        for (int i = 0; i < voices.size(); ++i) {
            auto o = std::dynamic_pointer_cast<dibiff::graph::MidiOutput>(_outputs[i]);
            std::vector<std::vector<unsigned char>> outputData;
            outputData.push_back(voices[i].midiMessage);
            o->setData(outputData, blockSize);
        }
        markProcessed();
    } else if (input->isConnected()) {
        std::vector<std::vector<unsigned char>> data = *input->getData();
        int blockSize = input->getBlockSize();
        /// Process the MIDI message and assign to voices
        for (auto message : data) {
            processMidiMessage(message);
        }
        /// Assign Voice outputs
        for (int i = 0; i < voices.size(); ++i) {
            auto o = std::dynamic_pointer_cast<dibiff::graph::MidiOutput>(_outputs[i]);
            std::vector<std::vector<unsigned char>> outputData;
            outputData.push_back(voices[i].midiMessage);
            o->setData(outputData, blockSize);
        }
        markProcessed();
    }
}
/**
 * @brief Check if the object is finished processing.
 */
bool dibiff::midi::VoiceSelector::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the object is ready to process.
 */
bool dibiff::midi::VoiceSelector::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}
/**
 * @brief Create a new instance of the object.
 * @param blockSize The block size of the object
 * @param numVoices The number of voices to create
 */
std::shared_ptr<dibiff::midi::VoiceSelector> dibiff::midi::VoiceSelector::create(int blockSize, int numVoices) {
    auto instance = std::make_shared<dibiff::midi::VoiceSelector>(blockSize, numVoices);
    instance->initialize();
    return instance;
}

void dibiff::midi::VoiceSelector::processMidiMessage(std::vector<unsigned char> message) {
    if (message.empty()) return;
    unsigned char status = message[0];
    unsigned char type = status & 0xF0;
    unsigned char noteNumber = message[1];
    unsigned char velocity = message[2];
    float frequency = midiNoteToFrequency(noteNumber);
    if (type == 0x90 && velocity > 0) { // Note on
        /// Find the next available voice and assign the frequency
        voices[voiceIndex].frequency = frequency;
        voices[voiceIndex].active = true;
        voices[voiceIndex].midiMessage = message;
        voiceIndex = (voiceIndex + 1) % voices.size();
    } else if (type == 0x80 || (type == 0x90 && velocity == 0)) { // Note off
        /// Find the voice with the matching frequency and deactivate it
        for (int i = 0; i < voices.size(); ++i) {
            if (voices[i].frequency == frequency) {
                voices[i].active = false;
                voices[i].midiMessage = message;
                // break;
            }
        }        
    }
}
/**
 * @brief Converts a MIDI note number to a frequency
 * @param noteNumber The MIDI note number
 */
float dibiff::midi::VoiceSelector::midiNoteToFrequency(int noteNumber) {
    return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
}