/// AudioPlayer.h

#pragma once

#include "sink.h"
#include "../graph/graph.h"
#include "miniaudio.h"
#include "../util/RingBuffer.h"

#include <vector>
#include <mutex>
#include <condition_variable>

/**
 * @brief Audio Player Sink
 * @details An Audio Player sink object that sends audio data to the default audio output device.
 */
class dibiff::sink::AudioPlayer : public dibiff::graph::AudioObject {
    public:
        std::condition_variable cv;
        std::mutex cv_mtx;
        std::shared_ptr<dibiff::graph::AudioInput> input;
        int sampleRate;
        int blockSize;
        ma_device device;
        std::unique_ptr<RingBuffer> ringBuffer;
        /**
         * @brief Get the name of the object
         * @return The name of the object
         */
        std::string getName() const override;

        /**
         * @brief Constructor
         * @details Initializes the Audio Player with a certain sample rate.
         * @param rate The sample rate of the audio data
         */
        AudioPlayer(int rate, int blockSize);

        /**
         * @brief Initialize
         * @details Initializes the Audio Player connection points and audio device
         */
        void initialize() override;

        /**
         * @brief Destructor
         * @details Finalizes the audio device and cleans up resources
         */
        ~AudioPlayer();

        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data and sends it to the audio output device
         */
        void process() override;

        /**
         * @brief Reset the Audio Player
         * @details Not used.
         */
        void reset() override {}

        /**
         * @brief Clear the Audio Player
         * @details Not used.
         */
        void clear() override {}

        /**
         * @brief Get the input connection point.
         * @return A shared pointer to the input connection point.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) override;

        /**
         * @brief Get the output connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) override;

        /**
         * @brief Get the reference connection point.
         * @return Not used.
         */
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getReference() override;

        /**
         * @brief Check if the Audio Player is finished processing
         * @return True if the Audio Player is finished processing, false otherwise
         */
        bool isFinished() const override;

        /**
         * @brief Check if the Audio Player is ready to process
         * @return True if the Audio Player is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;

        /**
         * @brief Creates a new Audio Player object
         * @param rate The sample rate of the audio data
         */
        static std::shared_ptr<AudioPlayer> create(int rate, int blockSize);

        /**
         * @brief Render the ImGui interface
         */
        void RenderImGui() override;

    private:
        std::vector<float> displaySamples;
        /**
         * @brief Audio playback callback
         * @details Callback function to provide audio data to the audio output device
         */
        static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
};