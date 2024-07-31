/// Reverb.h

#pragma once

#include "effect.h"
#include "../graph/graph.h"

/**
 * @brief Reverb
 * @details A reverb object is a simple object that adds a reverb effect to
 * the input signal. The reverb object has a certain decay time and a certain
 * room size.
 * @param decayTime The decay time of the reverb in milliseconds
 * @param roomSize The room size of the reverb in meters
 * @param sampleRate The sample rate of the input signal
 * @return A reverb object
 */
class dibiff::effect::Reverb : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::shared_ptr<dibiff::graph::AudioOutput> output;
    public:
        /**
         * @brief Constructor
         * @details Initializes the reverb object with a certain decay time
         * and room size
         * @param decayTime The decay time of the reverb in seconds
         * @param roomSize The room size of the reverb in meters
         * @param sampleRate The sample rate of the input signal
         * @param numDelays The number of delay lines
         * @param wetLevel The wet level of the reverb
         */
        Reverb(float decayTime, float roomSize, float sampleRate, int numDelays = 4, float wetLevel = 0.5f);
        /**
         * @brief Initialize
         * @details Initializes the reverb connection points and delay buffers
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
         * @brief Reset the reverb
         * @details Resets the delay buffers
         */
        void reset() override;
        /**
         * @brief Clear the reverb
         * @details Clears the delay buffers
         */
        void clear() override;
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
         * @brief Check if the reverb is finished processing
         * @return True if the reverb is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the filter is ready to process
         * @return True if the filter is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * Create a new reverb object
         * @param decayTime The decay time of the reverb in seconds
         * @param roomSize The room size of the reverb in meters
         * @param sampleRate The sample rate of the input signal
         * @param numDelays The number of delay lines
         * @param wetLevel The wet level of the reverb
         */
        static std::shared_ptr<Reverb> create(float decayTime, float roomSize, float sampleRate, int numDelays = 4, float wetLevel = 0.5f);
        /**
         * @brief Render the ImGui interface
         */
        void RenderImGui() override;
    private:
        std::vector<float> displayInSamples;
        std::vector<float> displayOutSamples;
        float decayTime;
        float roomSize;
        float sampleRate;
        float wetLevel;
        float feedback;
        int numDelays;
        const float speedOfSound = 343.0f;
        std::vector<std::vector<float>> buffers;
        std::vector<int> bufferIndices;
        float lastDecayTime;
        float lastRoomSize;
};