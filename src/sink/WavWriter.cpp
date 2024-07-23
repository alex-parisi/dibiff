/// WavWriter.cpp

#include "WavWriter.h"

/**
 * @brief Get the name of the object
 * @return The name of the object
 */
std::string dibiff::sink::WavWriter::getName() const { return "WavWriter"; }
/**
 * @brief Constructor
 * @details Initializes the WAV sink with a certain filename, sample rate,
 * and total number of samples
 * @param filename The filename of the WAV file
 * @param rate The sample rate of the WAV file
 */
dibiff::sink::WavWriter::WavWriter(const std::string& filename, int rate)
    : filename(filename), sampleRate(rate), writtenSamples(0) {}
/**
 * @brief Initialize
 * @details Initializes the WAV sink connection points and opens the WAV file
 */
void dibiff::sink::WavWriter::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "WavWriterInput");
    file.open(filename, std::ios::binary);
    if (file.is_open()) {
        writeHeader();
    }
}
/**
 * @brief Destructor
 * @details Closes the WAV file and finalizes the header
 */
dibiff::sink::WavWriter::~WavWriter() {
    if (file.is_open()) {
        finalizeHeader();
        file.close();
    }
}
/**
 * @brief Process a block of samples
 * @details Processes a block of audio data
 */
void dibiff::sink::WavWriter::process() {
    if (input->isReady()) {
        auto audioData = input->getData();
        int blockSize = input->getBlockSize();
        for (int i = 0; i < blockSize; ++i) {
            int16_t intSample = static_cast<int16_t>((*audioData)[i] * 32767);
            file.write(reinterpret_cast<const char*>(&intSample), sizeof(int16_t));
            writtenSamples++;
        }
        markProcessed();
    }
}
/**
 * @brief Get the input connection point.
 * @return A shared pointer to the input connection point.
 */
std::weak_ptr<dibiff::graph::AudioInput> dibiff::sink::WavWriter::getInput() { return input; }
/**
 * @brief Get the output connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioOutput> dibiff::sink::WavWriter::getOutput() { return std::weak_ptr<dibiff::graph::AudioOutput>(); }
/**
 * @brief Get the reference connection point.
 * @return Not used.
 */
std::weak_ptr<dibiff::graph::AudioReference> dibiff::sink::WavWriter::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); }
/**
 * @brief Check if the WavSink is finished processing
 * @return True if the WavSink is finished processing, false otherwise
 */
bool dibiff::sink::WavWriter::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}
/**
 * @brief Check if the WAV sink is ready to process
 * @return True if the WAV sink is ready to process, false otherwise
 */
bool dibiff::sink::WavWriter::isReadyToProcess() const {
    return input->isConnected() && input->isReady() && !processed;
}
/**
 * @brief Finalize the WAV header
 * @details Finalizes the WAV header by writing the chunk sizes
 */
void dibiff::sink::WavWriter::finalizeHeader() {
    size_t fileLength = writtenSamples * 2 + 44;
    file.seekp(4, std::ios::beg);
    writeWord(fileLength - 8, 4);
    file.seekp(40, std::ios::beg);
    writeWord(writtenSamples * 2, 4);
}
/**
 * @brief Creates a new WAV sink object
 * @param filename The filename of the WAV file
 * @param rate The sample rate of the WAV file
 */
std::shared_ptr<dibiff::sink::WavWriter> dibiff::sink::WavWriter::create(const std::string& filename, int rate) {
    auto instance = std::make_shared<dibiff::sink::WavWriter>(filename, rate);
    instance->initialize();
    return instance;
}
/**
 * @brief Write the WAV header
 * @details Writes the WAV header to the file
 */
void dibiff::sink::WavWriter::writeHeader() {
    file << "RIFF----WAVEfmt "; // (chunk size to be filled in later)
    writeWord(16, 4); // no extension data
    writeWord(1, 2);  // PCM - integer samples
    writeWord(1, 2);  // one channel (mono file)
    writeWord(sampleRate, 4); // samples per second (Hz)
    writeWord(sampleRate * 2, 4); // (Sample Rate * BitsPerSample * Channels) / 8
    writeWord(2, 2);  // data block size (size of two integer samples, one for each channel, in bytes)
    writeWord(16, 2); // number of bits per sample (use a multiple of 8)
    // Write the data chunk header
    file << "data----";  // (chunk size to be filled in later)
}
/**
 * @brief Write a word to the file
 * @details Writes a word to the file
 */
void dibiff::sink::WavWriter::writeWord(uint32_t value, unsigned size) {
    for (; size; --size, value >>= 8) {
        file.put(static_cast<char>(value & 0xFF));
    }
}