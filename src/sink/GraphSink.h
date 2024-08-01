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
        std::shared_ptr<dibiff::graph::AudioInput> input;
        int sampleRate;
        int blockSize;
        std::shared_ptr<RingBuffer<float>> ringBuffer;

        /**
         * @brief Constructor
         * @details Initializes the GraphSink with a certain sample rate.
         * @param rate The sample rate of the audio data
         */
        GraphSink(int rate, int blockSize);

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
         * @param rate The sample rate of the audio data
         */
        static std::shared_ptr<GraphSink> create(int rate, int blockSize);
};