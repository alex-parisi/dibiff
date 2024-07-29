/// SampleGenerator.cpp

#include "SampleGenerator.h"

std::string dibiff::generator::SampleGenerator::getName() const { return "SampleGenerator"; }

dibiff::generator::SampleGenerator::SampleGenerator(std::string filename, int blockSize, int sampleRate)
: dibiff::generator::Generator(), filename(filename), blockSize(blockSize), sampleRate(sampleRate), currentSample(-1) {}

void dibiff::generator::SampleGenerator::initialize() {
    input = std::make_shared<dibiff::graph::MidiInput>(dibiff::graph::MidiInput(shared_from_this(), "SampleGeneratorMidiInput"));
    /// Load the samples from the file
    loadSamples(filename);
    /// Create the audio output connection points
    for (int i = 0; i < samples.size(); ++i) {
        outputs.push_back(std::make_shared<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(shared_from_this(), "SampleGeneratorOutput" + std::to_string(i))));
    }
}

void dibiff::generator::SampleGenerator::loadSamples(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error opening the WAV file.");
    }

    // Read the RIFF header
    char riffHeader[4];
    file.read(riffHeader, 4);
    if (std::strncmp(riffHeader, "RIFF", 4) != 0) {
        throw std::runtime_error("Not a valid WAV file.");
    }

    // Skip the next 4 bytes (file size) and read the "WAVE" identifier
    file.seekg(4, std::ios::cur);
    char waveHeader[4];
    file.read(waveHeader, 4);
    if (std::strncmp(waveHeader, "WAVE", 4) != 0) {
        throw std::runtime_error("Not a valid WAV file.");
    }

    // Read the chunks until "data" chunk is found
    char chunkHeader[4];
    uint32_t chunkSize;
    uint16_t audioFormat, numChannels;
    uint32_t sampleRate, byteRate;
    uint16_t blockAlign, bitsPerSample;

    while (file.read(chunkHeader, 4)) {
        file.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
        if (std::strncmp(chunkHeader, "fmt ", 4) == 0) {
            file.read(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat));
            file.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
            file.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
            file.read(reinterpret_cast<char*>(&byteRate), sizeof(byteRate));
            file.read(reinterpret_cast<char*>(&blockAlign), sizeof(blockAlign));
            file.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));
            // Skip any remaining bytes in the fmt chunk if there are any
            file.seekg(chunkSize - 16, std::ios::cur);
        } else if (std::strncmp(chunkHeader, "data", 4) == 0) {
            uint32_t dataSize = chunkSize;
            uint32_t totalSamples = dataSize / (numChannels * (bitsPerSample / 8));
            samples.resize(numChannels, Eigen::VectorXf(totalSamples));
            // Read the sample data
            for (int frame = 0; frame < totalSamples; ++frame) {
                for (int channel = 0; channel < numChannels; ++channel) {
                    float floatSample = 0.0f;
                    if (audioFormat == 3) {  // IEEE float
                        file.read(reinterpret_cast<char*>(&floatSample), sizeof(float));
                    } else {
                        int32_t sample = 0;
                        file.read(reinterpret_cast<char*>(&sample), bitsPerSample / 8);
                        // Handle bit-depth conversion for both signed and unsigned formats
                        switch (bitsPerSample) {
                            case 8:
                                floatSample = (static_cast<uint8_t>(sample) - 128) / 128.0f;
                                break;
                            case 16:
                                floatSample = sample / 32768.0f; // 2^15
                                break;
                            case 24:
                                floatSample = sample / 8388608.0f; // 2^23
                                break;
                            case 32:
                                floatSample = sample / 2147483648.0f; // 2^31
                                break;
                            default:
                                floatSample = 0.0f; // Default case, unsupported bit depth
                                break;
                        }
                    }
                    samples[channel][frame] = floatSample;
                }
            }
            file.close();
            return;  // Successfully read data, exit the function
        } else {
            // Skip this unknown chunk
            file.seekg(chunkSize, std::ios::cur);
        }
    }
    file.close();
    throw std::runtime_error("Data chunk not found.");
}

int dibiff::generator::SampleGenerator::hasNoteOnNoteOff(std::vector<unsigned char> message) {
    if (message.empty()) return 0;
    unsigned char status = message[0];
    unsigned char type = status & 0xF0;
    unsigned char velocity = message[2];
    if (type == 0x90 && velocity > 0) { // Note on
        return 1;
    } else if (type == 0x80 || (type == 0x90 && velocity == 0)) { // Note off
        return -1;
    }
    return 0;
}

void dibiff::generator::SampleGenerator::process() {
    if (input->isConnected()) {
        auto midiData = *input->getData();
        int noteOnOff = 0;
        for (const auto& message : midiData) {
            noteOnOff += hasNoteOnNoteOff(message);
        }
        if (noteOnOff > 0) {
            currentSample = 0;
        }
    }
    if (currentSample == -1) {
        /// We're not generating samples, so output zeros
        for (int i = 0; i < outputs.size(); ++i) {
            std::vector<float> out(blockSize, 0.0f);
            outputs[i]->setData(out, out.size());
        }
    } else {
        /// Generate samples from the loaded samples
        for (int i = 0; i < outputs.size(); ++i) {
            int remainingSamples = samples[i].size() - currentSample;
            if (remainingSamples > 0) {
                int actualBlockSize = std::min(blockSize, remainingSamples);
                Eigen::VectorXf out = samples[i].segment(currentSample, actualBlockSize);
                std::vector<float> outVec(out.data(), out.data() + out.size());
                // Zero-pad if the actual block size is less than the requested block size
                if (actualBlockSize < blockSize) {
                    outVec.resize(blockSize, 0.0f);
                }
                outputs[i]->setData(outVec, outVec.size());
            } else {
                // If no samples are remaining, just set a zero-padded vector
                std::vector<float> outVec(blockSize, 0.0f);
                outputs[i]->setData(outVec, outVec.size());
            }
        }
        currentSample += blockSize;
    }
    markProcessed();
}

void dibiff::generator::SampleGenerator::reset() {
    currentSample = 0;
    processed = false;
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SampleGenerator::getInput(int i) { return input; };

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SampleGenerator::getOutput(int i) { return outputs[i]; }

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::generator::SampleGenerator::getReference() { return std::weak_ptr<dibiff::graph::AudioReference>(); };

bool dibiff::generator::SampleGenerator::isReadyToProcess() const {
    return !processed;
}

bool dibiff::generator::SampleGenerator::isFinished() const {
    return false;
}

std::shared_ptr<dibiff::generator::SampleGenerator> dibiff::generator::SampleGenerator::create(std::string filename, int blockSize, int sampleRate) {
    auto instance = std::make_shared<dibiff::generator::SampleGenerator>(filename, blockSize, sampleRate);
    instance->initialize();
    return instance;
}