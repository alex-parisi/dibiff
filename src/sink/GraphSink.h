/// AudioPlayer.h

#pragma once

#include "sink.h"
#include "../graph/graph.h"
#include "../util/RingBuffer.h"

#include <vector>
#include <mutex>
#include <condition_variable>

/**
 * @brief Graph Sink
 * @details A graph sink object that sends audio data to the thread responsible for handling
 * the audio data.
 */
class dibiff::sink::GraphSink : public dibiff::graph::AudioObject {
    public:
        std::condition_variable cv;
        std::mutex cv_mtx;
        int sampleRate;
        int blockSize;
        int channels;
        std::vector<std::unique_ptr<RingBuffer<float>>> ringBuffers;

        /**
         * @brief Constructor
         * @details Initializes the GraphSink with a certain sample rate.
         * @param channels The number of channels in the audio data
         * @param rate The sample rate of the audio data
         * @param blockSize The block size of the audio data
         */
        GraphSink(int channels, int rate, int blockSize);

        /**
         * @brief Initialize
         * @details Initializes the GraphSink connection points and audio device
         */
        void initialize() override;

        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data and sends it to the audio output device
         */
        void process() override;

        /**
         * @brief Reset the GraphSink
         * @details Not used.
         */
        void reset() override {}

        /**
         * @brief Clear the GraphSink
         * @details Not used.
         */
        void clear() override {}

        /**
         * @brief Check if the GraphSink is finished processing
         * @return True if the GraphSink is finished processing, false otherwise
         */
        bool isFinished() const override;

        /**
         * @brief Check if the GraphSink is ready to process
         * @return True if the GraphSink is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;

        /**
         * @brief Creates a new GraphSink object
         * @param channels The number of channels in the audio data
         * @param rate The sample rate of the audio data
         * @param blockSize The block size of the audio data
         */
        static std::unique_ptr<GraphSink> create(int channels, int rate, int blockSize);
};