/// graph.cpp

#include "graph.h"

#include <queue>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <iostream>

#include "../generator/generator.h"
#include "../sink/GraphSink.h"

std::vector<float> dibiff::graph::AudioInput::empty = {};
std::vector<std::vector<unsigned char>> dibiff::graph::MidiInput::empty = {};

/**
 * Audio Input implementation
 */
void dibiff::graph::AudioInput::connect(dibiff::graph::AudioOutput* output) {
    connectedOutput = output;
}
void dibiff::graph::AudioInput::disconnect() {
    connectedOutput = nullptr;
}
bool dibiff::graph::AudioInput::isConnected() {
    return (connectedOutput != nullptr);
}
bool dibiff::graph::AudioInput::isReady() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->isProcessed();
    }
    return false;
}
bool dibiff::graph::AudioInput::isFinished() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->isFinished();
    }
    return false;
}
const std::vector<float>& dibiff::graph::AudioInput::getData() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->getData();
    }
    return empty;
}
const int dibiff::graph::AudioInput::getBlockSize() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->getBlockSize();
    }
    return 0;
}
/**
 * MIDI Input implementation
 */
void dibiff::graph::MidiInput::connect(dibiff::graph::MidiOutput* output) {
    connectedOutput = output;
}
void dibiff::graph::MidiInput::disconnect() {
    connectedOutput = nullptr;
}
bool dibiff::graph::MidiInput::isConnected() {
    return (connectedOutput != nullptr);
}
bool dibiff::graph::MidiInput::isReady() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->isProcessed();
    }
    return false;
}
bool dibiff::graph::MidiInput::isFinished() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->isFinished();
    }
    return false;
}
const std::vector<std::vector<unsigned char>>& dibiff::graph::MidiInput::getData() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->getData();
    }
    return empty;
}
const int dibiff::graph::MidiInput::getBlockSize() const {
    if (connectedOutput != nullptr) {
        return connectedOutput->getBlockSize();
    }
    return 0;
}
/**
 * Audio Output implementation
 */
bool dibiff::graph::AudioOutput::isProcessed() const {
    return parent->isProcessed();
}
bool dibiff::graph::AudioOutput::isFinished() const {
    return parent->isFinished();
}
void dibiff::graph::AudioOutput::setData(std::vector<float> audioData, int N) {
    data = audioData;
    blockSize = N;
}
const std::vector<float>& dibiff::graph::AudioOutput::getData() const {
    return data;
}
const int dibiff::graph::AudioOutput::getBlockSize() const {
    return blockSize;
}
void dibiff::graph::AudioOutput::connect(dibiff::graph::AudioInput* inChannel) {
    if (!inChannel->isConnected()) {
        inChannel->connect(this);
        connectedInputs.push_back(inChannel);
    } else {
        throw std::runtime_error("Input already connected.");
    }
}
void dibiff::graph::AudioOutput::disconnect(dibiff::graph::AudioInput* inChannel) {
    if (inChannel->isConnected()) {
        inChannel->disconnect();
    }
    auto it = std::remove(connectedInputs.begin(), connectedInputs.end(), inChannel);
    if (it != connectedInputs.end()) {
        connectedInputs.erase(it, connectedInputs.end());
    }
}
void dibiff::graph::AudioOutput::disconnect() {
    for (auto& inChannel : connectedInputs) {
        if (inChannel->isConnected()) {
            inChannel->disconnect();
        }
    }
    connectedInputs.clear();
}

bool dibiff::graph::AudioOutput::isConnected() {
    return !connectedInputs.empty();
}

/**
 * MIDI Output implementation
 */
bool dibiff::graph::MidiOutput::isProcessed() const {
    return parent->isProcessed();
}
bool dibiff::graph::MidiOutput::isFinished() const {
    return parent->isFinished();
}
void dibiff::graph::MidiOutput::setData(std::vector<std::vector<unsigned char>> audioData, int N) {
    data = audioData;
    blockSize = N;
}
const std::vector<std::vector<unsigned char>>& dibiff::graph::MidiOutput::getData() const {
    return data;
}
const int dibiff::graph::MidiOutput::getBlockSize() const {
    return blockSize;
}
void dibiff::graph::MidiOutput::connect(dibiff::graph::MidiInput* inChannel) {
    if (!inChannel->isConnected()) {
        inChannel->connect(this);
        connectedInputs.push_back(inChannel);
    } else {
        throw std::runtime_error("Input already connected.");
    }
}
void dibiff::graph::MidiOutput::disconnect(dibiff::graph::MidiInput* inChannel) {
    if (inChannel->isConnected()) {
        inChannel->disconnect();
    }
    /// Also remove this from the vector of connectedInputs
    auto it = std::remove(connectedInputs.begin(), connectedInputs.end(), inChannel);
    if (it != connectedInputs.end()) {
        connectedInputs.erase(it, connectedInputs.end());
    }
}
void dibiff::graph::MidiOutput::disconnect() {
    for (auto& inChannel : connectedInputs) {
        if (inChannel->isConnected()) {
            inChannel->disconnect();
        }
    }
    connectedInputs.clear();
}
bool dibiff::graph::MidiOutput::isConnected() {
    return !connectedInputs.empty();
}
/**
 * @brief Audio Graph
 * @details An audio graph is a collection of audio objects that are connected
 * together to form a processing graph. The audio graph processes the audio
 * objects in the correct order to generate the final output.
 */
dibiff::graph::AudioObject* dibiff::graph::AudioGraph::add(dibiff::graph::AudioObject* obj) {
    objects.push_back(obj);
    return obj;
}
dibiff::graph::AudioCompositeObject* dibiff::graph::AudioGraph::add(dibiff::graph::AudioCompositeObject* obj) {
    for (auto& o : obj->objects) {
        objects.push_back(o.get());
    }
    return obj;
}
void dibiff::graph::AudioGraph::remove(dibiff::graph::AudioObject* obj) {
    /// Remove this object from the vector of objects
    objects.erase(std::remove_if(objects.begin(), objects.end(), [&](const dibiff::graph::AudioObject* o) {
        return o == obj;
    }), objects.end());
}
void dibiff::graph::AudioGraph::remove(dibiff::graph::AudioCompositeObject* obj) {
    for (auto& o : obj->objects) {
        remove(o.get());
    }
}
/**
 * @brief Process the audio graph
 * @details Processes the audio graph by running the audio objects in the correct order
 * and connecting the audio objects together. The audio graph processes the audio objects
 * in a block-based manner, where each audio object processes a block of samples at a time.
 * This is a multi-threaded implementation where each audio object is processed in a separate
 * thread.
 */
void dibiff::graph::AudioGraph::tick() {
    std::queue<dibiff::graph::AudioObject*> readyQueue;
    std::unordered_set<dibiff::graph::AudioObject*> processed;
    std::unordered_set<dibiff::graph::AudioObject*> inQueueOrProcessed;
    std::mutex queueMutex;
    std::mutex processedMutex;
    // Initialize the queue with objects that are ready to process
    for (auto& obj : objects) {
        // Mark all objects as not processed at the start of each block
        obj->markProcessed(false);
    }
    /// Must do this twice to prevent out-of-order processing
    for (auto& obj : objects) {
        if (obj->isReadyToProcess()) {
            readyQueue.push(obj);
            inQueueOrProcessed.insert(obj);
        }
    }
    if (readyQueue.empty()) {
        /// No objects to process.
        return;
    }
    while (!readyQueue.empty()) {
        std::vector<std::thread> threads;
        while (!readyQueue.empty()) {
            auto obj = readyQueue.front();
            readyQueue.pop();
            // Create a thread to process the object
            threads.push_back(std::thread([obj, &processedMutex, &processed, &inQueueOrProcessed]() {
                obj->process();
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
}

/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param outChannel The output channel
 * @param inChannel The input channel
 */
void dibiff::graph::AudioGraph::connect(dibiff::graph::AudioOutput* outChannel, dibiff::graph::AudioInput* inChannel) {
    if (!inChannel->isConnected()) {
        outChannel->connect(inChannel);
    } else {
        throw std::runtime_error("Input already connected.");
    }
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param inChannel The input channel
 * @param outChannel The output channel
 */
void dibiff::graph::AudioGraph::connect(dibiff::graph::AudioInput* inChannel, dibiff::graph::AudioOutput* outChannel) {
    connect(outChannel, inChannel);
}
/**
 * @brief Connect two audio objects
 * @details Connects two audio objects together
 * @param refChannel The reference channel
 * @param outChannel The output channel
 */
void dibiff::graph::AudioGraph::connect(dibiff::graph::AudioConnectionPoint* pt1, dibiff::graph::AudioConnectionPoint* pt2) {
    /// MIDI Out -> MIDI In?
    if (auto pt1Midi = dynamic_cast<dibiff::graph::MidiOutput*>(pt1)) {
        if (auto pt2Midi = dynamic_cast<dibiff::graph::MidiInput*>(pt2)) {
            pt1Midi->connect(pt2Midi);
            return;
        }
    }
    /// MIDI In -> MIDI Out?
    if (auto pt1Midi = dynamic_cast<dibiff::graph::MidiInput*>(pt1)) {
        if (auto pt2Midi = dynamic_cast<dibiff::graph::MidiOutput*>(pt2)) {
            pt2Midi->connect(pt1Midi);
            return;
        }
    }
    /// Audio In -> Audio Out?
    if (auto pt1Audio = dynamic_cast<dibiff::graph::AudioInput*>(pt1)) {
        if (auto pt2Audio = dynamic_cast<dibiff::graph::AudioOutput*>(pt2)) {
            pt2Audio->connect(pt1Audio);
            return;
        }
    }
    /// Audio Out -> Audio In?
    if (auto pt1Audio = dynamic_cast<dibiff::graph::AudioOutput*>(pt1)) {
        if (auto pt2Audio = dynamic_cast<dibiff::graph::AudioInput*>(pt2)) {
            pt1Audio->connect(pt2Audio);
            return;
        }
    }
    /// Incorrect, throw error
    throw std::runtime_error("Invalid connection.");
}

void dibiff::graph::AudioGraph::disconnect(dibiff::graph::AudioOutput* outChannel, dibiff::graph::AudioInput* inChannel) {
    if (inChannel->isConnected()) {
        outChannel->disconnect(inChannel);
    }
}

void dibiff::graph::AudioGraph::disconnect(dibiff::graph::AudioInput* inChannel, dibiff::graph::AudioOutput* outChannel) {
    disconnect(outChannel, inChannel);
}

void dibiff::graph::AudioGraph::disconnect(dibiff::graph::AudioConnectionPoint* pt1, dibiff::graph::AudioConnectionPoint* pt2) {
    /// MIDI Out -> MIDI In?
    if (auto pt1Midi = dynamic_cast<dibiff::graph::MidiOutput*>(pt1)) {
        if (auto pt2Midi = dynamic_cast<dibiff::graph::MidiInput*>(pt2)) {
            pt1Midi->disconnect(pt2Midi);
            return;
        }
    }
    /// MIDI In -> MIDI Out?
    if (auto pt1Midi = dynamic_cast<dibiff::graph::MidiInput*>(pt1)) {
        if (auto pt2Midi = dynamic_cast<dibiff::graph::MidiOutput*>(pt2)) {
            pt2Midi->disconnect(pt1Midi);
            return;
        }
    }
    /// Audio In -> Audio Out?
    if (auto pt1Audio = dynamic_cast<dibiff::graph::AudioInput*>(pt1)) {
        if (auto pt2Audio = dynamic_cast<dibiff::graph::AudioOutput*>(pt2)) {
            pt2Audio->disconnect(pt1Audio);
            return;
        }
    }
    /// Audio Out -> Audio In?
    if (auto pt1Audio = dynamic_cast<dibiff::graph::AudioOutput*>(pt1)) {
        if (auto pt2Audio = dynamic_cast<dibiff::graph::AudioInput*>(pt2)) {
            pt1Audio->disconnect(pt2Audio);
            return;
        }
    }
    /// Incorrect, throw error
    throw std::runtime_error("Invalid disconnection.");
}