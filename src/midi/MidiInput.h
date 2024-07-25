/// MidiInput.h

#pragma once

#include "midi.h"
#include "../graph/graph.h"

#include "RtMidi.h"

/**
 * @brief MIDI Input
 * @details A MIDI input object that generates MIDI data
 */
class dibiff::midi::MidiInput : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    std::unique_ptr<RtMidiIn> midiIn;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Constructs a MIDI input object
         */
        MidiInput(int blockSize, int portNum = 0);
        /**
         * @brief Initialize
         * @details Initializes the MIDI input object
         */
        void initialize() override;
        /**
         * @brief Generate a block of samples
         * @details Generates a block of MIDI data
         */
        void process() override;
        /**
         * @brief Reset the MIDI input
         * @details Not used.
         */
        void reset() override {};
        /**
         * @brief Clear the MIDI input
         * @details Not used.
         */
        void clear() override {}
        /**
         * @brief Get the input connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioInput> getInput(int i = 0) override;
        /**
         * @brief Get the output connection point.
         * @return A shared pointer to the output connection point.
         */
        std::weak_ptr<dibiff::graph::AudioOutput> getOutput() override;
        /**
         * @brief Get the reference connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioReference> getReference() override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * @brief Check if the sine wave source is finished
         * @return True if the sine wave source has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new MIDI input object
         */
        static std::shared_ptr<MidiInput> create(int blockSize, int portNum = 0);
    private:
        int blockSize;
        int portNum;
        std::string getNoteName(int noteNumber);
        void processMidiMessage(const std::vector<unsigned char>& message);
};