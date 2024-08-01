/// MidiInput.h

#pragma once

#include "midi.h"
#include "../graph/graph.h"
#include <vector>
#include <mutex>

/**
 * @brief MIDI Input
 * @details A MIDI input object that generates MIDI data
 */
class dibiff::midi::MidiInput : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::MidiOutput> output;
    public:
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
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) override;
        /**
         * @brief Get the output connection point.
         * @return A shared pointer to the output connection point.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) override;
        /**
         * @brief Get the reference connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getReference() override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * @brief Check if the MIDI input has finished generating samples
         * @return True if the MIDI input has finished generating samples, false otherwise
         */
        bool isFinished() const override;
        /**
         * Create a new MIDI input object
         */
        static std::shared_ptr<dibiff::midi::MidiInput> create(int blockSize, int portNum = 0);
    private:
        int blockSize;
        int portNum;
};