/// WavWriter.h

#pragma once

#include "sink.h"
#include "../graph/graph.h"
#include <fstream>

/**
 * @brief WAV Sink
 * @details A WAV sink object is a simple object that writes audio data to a
 * WAV file. The WAV sink object has a certain filename and sample rate.
 */
class dibiff::sink::WavWriter : public dibiff::graph::AudioObject {
    std::shared_ptr<dibiff::graph::AudioInput> input;
    std::ofstream file;
    int sampleRate;
    size_t writtenSamples;
    std::string filename;
    public:
        /**
         * @brief Constructor
         * @details Initializes the WAV sink with a certain filename, sample rate,
         * and total number of samples
         * @param filename The filename of the WAV file
         * @param rate The sample rate of the WAV file
         */
        WavWriter(const std::string& filename, int rate);
        /**
         * @brief Initialize
         * @details Initializes the WAV sink connection points and opens the WAV file
         */
        void initialize() override;
        /**
         * @brief Destructor
         * @details Closes the WAV file and finalizes the header
         */
        ~WavWriter();
        /**
         * @brief Process a block of samples
         * @details Processes a block of audio data
         */
        void process() override;
        /**
         * @brief Reset the WAV sink
         * @details Not used.
         */
        void reset() override {}
        /**
         * @brief Clear the WAV sink
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
         * @brief Check if the WavSink is finished processing
         * @return True if the WavSink is finished processing, false otherwise
         */
        bool isFinished() const override;
        /**
         * @brief Check if the WAV sink is ready to process
         * @return True if the WAV sink is ready to process, false otherwise
         */
        bool isReadyToProcess() const override;
        /**
         * @brief Finalize the WAV header
         * @details Finalizes the WAV header by writing the chunk sizes
         */
        void finalizeHeader();
        /**
         * @brief Creates a new WAV sink object
         * @param filename The filename of the WAV file
         * @param rate The sample rate of the WAV file
         */
        static std::shared_ptr<WavWriter> create(const std::string& filename, int rate);
    private:
        /**
         * @brief Write the WAV header
         * @details Writes the WAV header to the file
         */
        void writeHeader();
        /**
         * @brief Write a word to the file
         * @details Writes a word to the file
         */
        void writeWord(uint32_t value, unsigned size);
};