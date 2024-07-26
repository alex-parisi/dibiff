/// Gain.h

#pragma once

#include "level.h"
#include "../graph/graph.h"

/**
 * @brief Gain
 * @details A static gain object is a simple object that multiplies the input by a
 * certain value. The value is in dB.
 * @param value The gain of the object in dB
 * @return A gain object
 */
class dibiff::level::Gain : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;
        /**
         * @brief Constructor
         * @details Initializes the gain object with a certain value
         * @param value The gain of the object in dB
         */
        Gain(float value);
        /**
         * @brief Initialize
         * @details Initializes the gain connection points
         */
        void initialize() override;
        /**
         * @brief Process a sample
         * @details Processes a single sample of audio data
         * @param sample The input sample
         */
        float process(float sample);
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
         * @brief Check if the gain is finished processing
         * @return True if the gain is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new gain object
         * @param value The gain of the object in dB
         */
        static std::shared_ptr<Gain> create(float value);
    private:
        float value;
};