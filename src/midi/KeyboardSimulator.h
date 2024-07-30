/// KeyboardSimulator.h

/// MidiInput.h

#pragma once

#include "midi.h"
#include "../graph/graph.h"

#include <vector>
#include <mutex>
#include <map>
#include <set>

/**
 * @brief MIDI Input
 * @details A MIDI input object that generates MIDI data
 */
class dibiff::midi::KeyboardSimulator : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::MidiOutput> output;
    std::vector<std::vector<unsigned char>> simulatedMidiEvents;
    public:
        /**
         * @brief Constructor
         * @details Constructs a MIDI input object
         */
        KeyboardSimulator(int blockSize);
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

        void RenderImGui() override;
        /**
         * Create a new MIDI input object
         */
        static std::shared_ptr<KeyboardSimulator> create(int blockSize);
    private:
        int blockSize;
        std::set<int> activeKeys;
        std::map<int, int> keyToMidiNote = {
            { ImGuiKey_A, 60 }, // Middle C
            { ImGuiKey_W, 61 }, // C#
            { ImGuiKey_S, 62 }, // D
            { ImGuiKey_E, 63 }, // D#
            { ImGuiKey_D, 64 }, // E
            { ImGuiKey_F, 65 }, // F
            { ImGuiKey_T, 66 }, // F#
            { ImGuiKey_G, 67 }, // G
            { ImGuiKey_Y, 68 }, // G#
            { ImGuiKey_H, 69 }, // A
            { ImGuiKey_U, 70 }, // A#
            { ImGuiKey_J, 71 }, // B
            { ImGuiKey_K, 72 }  // High C
            // Add more mappings as needed
        };
};