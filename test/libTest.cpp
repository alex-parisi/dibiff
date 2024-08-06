#include "../dibiff"
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <cmath>

int main() {
    int sampleRate = 48000;
    int blockSize = 512;

    dibiff::graph::AudioGraph graph;

    // Create MIDI Input
    auto midiInput = graph.add(dibiff::midi::MidiInput::create(blockSize));
    midiInput->setName("midi-input");

    // Create Sine Generator
    auto sineGenerator = graph.add(dibiff::generator::SineGenerator::create(blockSize, sampleRate));
    sineGenerator->setName("sine-generator");

    // Create AudioPlayer
    auto audioPlayer = graph.add(dibiff::sink::GraphSink::create(1, sampleRate, blockSize));
    audioPlayer->setName("audio-player");

    // Connect everything
    graph.connect(midiInput->getOutput(), sineGenerator->getInput());
    graph.connect(sineGenerator->getOutput(), audioPlayer->getInput());

    // Create a condition variable and mutex for synchronization
    std::condition_variable cv;
    std::mutex cv_mtx;

    // Start the audio graph in a separate thread
    std::thread audioThread([&graph]() {
        graph.run(true);
    });

    // Start MIDI event generation in a separate thread
    std::thread midiThread([&midiInput, blockSize]() {
        auto mi = std::dynamic_pointer_cast<dibiff::midi::MidiInput>(midiInput);
        while (true) {
            // Simulate generating a MIDI message (example: Note On)
            std::vector<unsigned char> midiMessage = {0x90, 0x40, 0x7F}; // Note On, middle C, velocity 127
            // Add the MIDI message to the input
            mi->addMidiMessage(midiMessage);
            std::cout << "MIDI message added" << std::endl;
            // Wait for a bit before generating the next MIDI event
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });

    // Start the audio data reading thread
    std::thread readThread([&audioPlayer]() {
        auto ap = std::dynamic_pointer_cast<dibiff::sink::GraphSink>(audioPlayer);
        std::vector<float> buffer(ap->blockSize);

        // Duration of each frame in microseconds
        const auto frameDuration = std::chrono::microseconds(static_cast<int>(std::round(1e6 / 48000.0)));

        while (true) {
            auto startTime = std::chrono::steady_clock::now();

            if (ap->ringBuffers[0]->available() > 0) {
                // Read audio data from the ring buffer
                size_t samplesRead = ap->ringBuffers[0]->read(buffer.data(), buffer.size());
                if (samplesRead > 0) {
                    // Print the audio data or process it
                }
                // Notify that data has been consumed
                ap->cv.notify_one();
            }
            // Calculate time taken for this iteration
            auto endTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

            // Sleep for the remaining time to maintain the 48000 Hz rate
            if (elapsedTime < frameDuration) {
                std::this_thread::sleep_for(frameDuration - elapsedTime);
            }
        }
    });

    // Join threads (for demonstration purposes; adjust as needed)
    audioThread.join();
    midiThread.join();
    readThread.join();

    return 0;
}