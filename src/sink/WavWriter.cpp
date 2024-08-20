/// WavWriter.cpp

#include "WavWriter.h"

/**
 * @brief Constructor
 * @details Initializes the WAV sink with a certain filename, sample rate,
 * and total number of samples
 * @param filename The filename of the WAV file
 * @param rate The sample rate of the WAV file
 */
dibiff::sink::WavWriter::WavWriter(const std::string& filename, int rate)
: dibiff::graph::AudioObject(), filename(filename), sampleRate(rate), writtenSamples(0) {
    name = "WavWriter";
}
/**
 * @brief Initialize
 * @details Initializes the WAV sink connection points and opens the WAV file
 */
void dibiff::sink::WavWriter::initialize() {
    auto i = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "WavWriterInput"));
    _inputs.emplace_back(std::move(i));
    input = static_cast<dibiff::graph::AudioInput*>(_inputs.back().get());
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
    if (!input->isConnected()) {
        /// Don't do anything if the input is not connected
        markProcessed();
    } else if (input->isReady()) {
        const auto& audioData = input->getData();
        const int blockSize = input->getBlockSize();
        /// Also write the audioData to the .wav file
        for (int i = 0; i < blockSize; ++i) {
            int16_t intSample = static_cast<int16_t>((audioData)[i] * 32767);
            file.write(reinterpret_cast<const char*>(&intSample), sizeof(int16_t));
            writtenSamples++;
        }
        markProcessed();
    }
}
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
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
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
std::unique_ptr<dibiff::sink::WavWriter> dibiff::sink::WavWriter::create(const std::string& filename, int rate) {
    auto instance = std::make_unique<dibiff::sink::WavWriter>(filename, rate);
    instance->initialize();
    return std::move(instance);
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