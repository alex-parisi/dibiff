/// graph.cpp

#include "graph.h"

#include <queue>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <iostream>

#include "../generator/generator.h"
#include "../sink/GraphSink.h"

/**
 * Audio Input implementation
 */
void dibiff::graph::AudioInput::connect(std::weak_ptr<dibiff::graph::AudioOutput> output) {
    connectedOutput = output;
}
bool dibiff::graph::AudioInput::isConnected() const {
    return !connectedOutput.expired();
}
bool dibiff::graph::AudioInput::isReady() const {
    if (auto output = connectedOutput.lock()) {
        return output->isProcessed();
    }
    return false;
}
bool dibiff::graph::AudioInput::isFinished() const {
    if (auto output = connectedOutput.lock()) {
        return output->isFinished();
    }
    return false;
}
std::optional<std::vector<float>> dibiff::graph::AudioInput::getData() const {
    if (auto output = connectedOutput.lock()) {
        return output->getData();
    }
    return std::nullopt;
}
int dibiff::graph::AudioInput::getBlockSize() const {
    if (auto output = connectedOutput.lock()) {
        return output->getBlockSize();
    }
    return 0;
}
/**
 * MIDI Input implementation
 */
void dibiff::graph::MidiInput::connect(std::weak_ptr<dibiff::graph::MidiOutput> output) {
    connectedOutput = output;
}
bool dibiff::graph::MidiInput::isConnected() const {
    return !connectedOutput.expired();
}
bool dibiff::graph::MidiInput::isReady() const {
    if (auto output = connectedOutput.lock()) {
        return output->isProcessed();
    }
    return false;
}
bool dibiff::graph::MidiInput::isFinished() const {
    if (auto output = connectedOutput.lock()) {
        return output->isFinished();
    }
    return false;
}
std::optional<std::vector<std::vector<unsigned char>>> dibiff::graph::MidiInput::getData() const {
    if (auto output = connectedOutput.lock()) {
        return output->getData();
    }
    return std::nullopt;
}
int dibiff::graph::MidiInput::getBlockSize() const {
    if (auto output = connectedOutput.lock()) {
        return output->getBlockSize();
    }
    return 0;
}
/**
 * Audio Reference implementation
 */
void dibiff::graph::AudioReference::connect(std::weak_ptr<dibiff::graph::AudioOutput> output) {
    connectedOutput = output;
}
bool dibiff::graph::AudioReference::isConnected() const {
    return !connectedOutput.expired();
}
bool dibiff::graph::AudioReference::isReady() const {
    if (auto output = connectedOutput.lock()) {
        return output->isProcessed();
    }
    return false;
}
bool dibiff::graph::AudioReference::isFinished() const {
    if (auto output = connectedOutput.lock()) {
        return output->isFinished();
    }
    return false;
}
std::optional<std::vector<float>> dibiff::graph::AudioReference::getData() const {
    if (auto output = connectedOutput.lock()) {
        return output->getData();
    }
    return std::nullopt;
}
int dibiff::graph::AudioReference::getBlockSize() const {
    if (auto output = connectedOutput.lock()) {
        return output->getBlockSize();
    }
    return 0;
}
/**
 * Audio Output implementation
 */
bool dibiff::graph::AudioOutput::isProcessed() const {
    if (auto p = parent.lock()) {
        return p->isProcessed();
    }
    return false;
}
bool dibiff::graph::AudioOutput::isFinished() const {
    if (auto p = parent.lock()) {
        return p->isFinished();
    }
    return false;
}
void dibiff::graph::AudioOutput::setData(std::vector<float>& audioData, int N) {
    data = audioData;
    blockSize = N;
}
std::vector<float> dibiff::graph::AudioOutput::getData() const {
    return data;
}
int dibiff::graph::AudioOutput::getBlockSize() const {
    return blockSize;
}
void dibiff::graph::AudioOutput::connect(std::weak_ptr<dibiff::graph::AudioInput> inChannel) {
    if (auto iC = inChannel.lock()) {
        if (!iC->isConnected()) {
            iC->connect(shared_from_this());
        } else {
            throw std::runtime_error("Input already connected.");
        }
    }
}
void dibiff::graph::AudioOutput::connect(std::weak_ptr<dibiff::graph::AudioReference> refChannel) {
    if (auto rC = refChannel.lock()) {
        if (!rC->isConnected()) {
            rC->connect(shared_from_this());
        } else {
            throw std::runtime_error("Reference already connected.");
        }
    }
}
/**
 * MIDI Output implementation
 */
bool dibiff::graph::MidiOutput::isProcessed() const {
    if (auto p = parent.lock()) {
        return p->isProcessed();
    }
    return false;
}
bool dibiff::graph::MidiOutput::isFinished() const {
    if (auto p = parent.lock()) {
        return p->isFinished();
    }
    return false;
}
void dibiff::graph::MidiOutput::setData(std::vector<std::vector<unsigned char>>& audioData, int N) {
    data = audioData;
    blockSize = N;
}
std::vector<std::vector<unsigned char>> dibiff::graph::MidiOutput::getData() const {
    return data;
}
int dibiff::graph::MidiOutput::getBlockSize() const {
    return blockSize;
}
void dibiff::graph::MidiOutput::connect(std::weak_ptr<dibiff::graph::MidiInput> inChannel) {
    if (auto iC = inChannel.lock()) {
        if (!iC->isConnected()) {
            iC->connect(shared_from_this());
        } else {
            throw std::runtime_error("Input already connected.");
        }
    }
}
/**
 * @brief Audio Graph
 * @details An audio graph is a collection of audio objects that are connected
 * together to form a processing graph. The audio graph processes the audio
 * objects in the correct order to generate the final output.
 */
std::shared_ptr<dibiff::graph::AudioObject> dibiff::graph::AudioGraph::add(std::shared_ptr<dibiff::graph::AudioObject> obj) {
    objects.push_back(obj);
    return obj;
}
std::shared_ptr<dibiff::graph::AudioCompositeObject> dibiff::graph::AudioGraph::add(std::shared_ptr<dibiff::graph::AudioCompositeObject> obj) {
    for (auto& o : obj->objects) {
        objects.push_back(o);
    }
    return obj;
}
/**
 * @brief Process the audio graph
 * @details Processes the audio graph by running the audio objects in the correct order
 * and connecting the audio objects together. The audio graph processes the audio objects
 * in a block-based manner, where each audio object processes a block of samples at a time.
 * This is a multi-threaded implementation where each audio object is processed in a separate
 * thread.
 */
void dibiff::graph::AudioGraph::run(bool realTime, int sampleRate, int blockSize) {
    const double blockDuration = static_cast<double>(blockSize) / sampleRate;
    int iter = 0;
    while (true) {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::queue<std::shared_ptr<dibiff::graph::AudioObject>> readyQueue;
        std::unordered_set<std::shared_ptr<dibiff::graph::AudioObject>> processed;
        std::unordered_set<std::shared_ptr<dibiff::graph::AudioObject>> inQueueOrProcessed;
        std::mutex queueMutex;
        std::mutex processedMutex;
        // Initialize the queue with objects that are ready to process
        for (auto& obj : objects) {
            // Mark all objects as not processed at the start of each block
            obj->markProcessed(false);
            if (obj->isReadyToProcess()) {
                readyQueue.push(obj);
                inQueueOrProcessed.insert(obj);
            }
        }
        if (readyQueue.empty()) {
            break; // No more objects to process
        }
        while (!readyQueue.empty()) {
            std::vector<std::thread> threads;
            while (!readyQueue.empty()) {
                auto obj = readyQueue.front();
                readyQueue.pop();
                // Create a thread to process the object
                threads.push_back(std::thread([obj, &processed, &inQueueOrProcessed, &processedMutex]() {
                    obj->process();
                    std::cout << "Processed: " << obj->getName() << std::endl;
                    std::lock_guard<std::mutex> lock(processedMutex);
                    processed.insert(obj);
                    inQueueOrProcessed.insert(obj);
                }));
            }
            // Join all threads
            for (auto& thread : threads) {
                thread.join();
            }
            // Check connected objects to see if they are now ready to process
            for (auto& connectedObj : objects) {
                std::lock_guard<std::mutex> lock(processedMutex);
                if (processed.find(connectedObj) == processed.end() &&
                    connectedObj->isReadyToProcess() &&
                    inQueueOrProcessed.find(connectedObj) == inQueueOrProcessed.end()) {
                    readyQueue.push(connectedObj);
                    inQueueOrProcessed.insert(connectedObj);
                }
            }
        }
        // Check if all generators are finished
        bool allSourcesFinished = true;
        for (auto& obj : objects) {
            if (auto o = std::dynamic_pointer_cast<dibiff::generator::Generator>(obj)) {
                if (!o->isFinished()) {
                    allSourcesFinished = false;
                    break;
                }
            }
        }
        if (allSourcesFinished) {
            std::cout << "All sources finished.\n";
            break;
        }
        iter++;
        if (realTime) {
            /// First, pull out the audio player object
            std::shared_ptr<dibiff::sink::GraphSink> graphSink = nullptr;
            for (auto& obj : objects) {
                if (auto o = std::dynamic_pointer_cast<dibiff::sink::GraphSink>(obj)) {
                    graphSink = std::dynamic_pointer_cast<dibiff::sink::GraphSink>(obj);
                    break;
                }
            }
            if (graphSink == nullptr) {
                std::cerr << "Warning: Audio player not found in real-time mode.\n";
                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = endTime - startTime;
                double sleepTime = blockDuration - elapsed.count();
                if (sleepTime > 0) {
                    std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
                }
            } else {
                std::cout << "Waiting for audio player to finish processing...\n";
                /// Wait for the audio player to finish processing
                std::unique_lock<std::mutex> lock(graphSink->cv_mtx);
                graphSink->cv.wait(lock);
            }
        }
    }
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param outChannel The output channel
 * @param inChannel The input channel
 */
void dibiff::graph::AudioGraph::connect(std::weak_ptr<dibiff::graph::AudioOutput> outChannel, std::weak_ptr<dibiff::graph::AudioInput> inChannel) {
    if (auto iC = inChannel.lock()) {
        if (!iC->isConnected()) {
            iC->connect(outChannel);
        } else {
            throw std::runtime_error("Input already connected.");
        }
    }
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param outChannel The output channel
 * @param refChannel The reference channel
 */
void dibiff::graph::AudioGraph::connect(std::weak_ptr<dibiff::graph::AudioOutput> outChannel, std::weak_ptr<dibiff::graph::AudioReference> refChannel) {
    if (auto rC = refChannel.lock()) {
        if (!rC->isConnected()) {
            rC->connect(outChannel);
        } else {
            throw std::runtime_error("Reference already connected.");
        }
    }
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param inChannel The input channel
 * @param outChannel The output channel
 */
void dibiff::graph::AudioGraph::connect(std::weak_ptr<dibiff::graph::AudioInput> inChannel, std::weak_ptr<dibiff::graph::AudioOutput> outChannel) {
    connect(outChannel, inChannel);
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param refChannel The reference channel
 * @param outChannel The output channel
 */
void dibiff::graph::AudioGraph::connect(std::weak_ptr<dibiff::graph::AudioReference> refChannel, std::weak_ptr<dibiff::graph::AudioOutput> outChannel) {
    connect(outChannel, refChannel);
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param refChannel The reference channel
 * @param outChannel The output channel
 */
void dibiff::graph::AudioGraph::connect(std::weak_ptr<dibiff::graph::AudioConnectionPoint> pt1, std::weak_ptr<dibiff::graph::AudioConnectionPoint> pt2) {
    /// MIDI Out -> MIDI In?
    if (auto pt1Midi = std::dynamic_pointer_cast<dibiff::graph::MidiOutput>(pt1.lock())) {
        if (auto pt2Midi = std::dynamic_pointer_cast<dibiff::graph::MidiInput>(pt2.lock())) {
            pt2Midi->connect(pt1Midi);
            return;
        }
    }
    /// MIDI In -> MIDI Out?
    if (auto pt1Midi = std::dynamic_pointer_cast<dibiff::graph::MidiInput>(pt1.lock())) {
        if (auto pt2Midi = std::dynamic_pointer_cast<dibiff::graph::MidiOutput>(pt2.lock())) {
            pt2Midi->connect(pt1Midi);
            return;
        }
    }
    /// Audio In -> Audio Out?
    if (auto pt1Audio = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(pt1.lock())) {
        if (auto pt2Audio = std::dynamic_pointer_cast<dibiff::graph::AudioOutput>(pt2.lock())) {
            pt2Audio->connect(pt1Audio);
            return;
        }
    }
    /// Audio Out -> Audio In?
    if (auto pt1Audio = std::dynamic_pointer_cast<dibiff::graph::AudioOutput>(pt1.lock())) {
        if (auto pt2Audio = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(pt2.lock())) {
            pt2Audio->connect(pt1Audio);
            return;
        }
    }
}