/// Mixer.h

#pragma once

#include "level.h"
#include "../graph/graph.h"

/**
 * @brief Mixer
 * @details A static mixer object is a simple object that multiplies the input by a
 * certain value. The value is in dB.
 * @param value The mixer of the object in dB
 * @return A mixer object
 */
class dibiff::level::Mixer : public dibiff::graph::AudioObject {
    std::vector<std::shared_ptr<dibiff::graph::AudioInput>> inputs;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the mixer object with a certain value
         */
        Mixer(int numInputs = 1);
        /**
         * @brief Initialize
         * @details Initializes the mixer connection points
         */
        void initialize() override;
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the envelope
         * @details Not used
         */
        void reset() override {}
        /**
         * @brief Clear the compressor
         * @details Not used
         */
        void clear() override {}
        /**
         * @brief Get the input connection point.
         * @return A shared pointer to the input connection point.
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
         * @brief Check if the mixer is finished processing
         * @return True if the mixer is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new mixer object
         * @param value The mixer of the object in dB
         */
        static std::shared_ptr<Mixer> create(int numInputs = 1);
    private:
        int numInputs;
};