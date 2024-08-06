/// AudioPlayer.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "GraphSink.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

dibiff::sink::GraphSink::GraphSink(int channels, int rate, int blockSize)
: dibiff::graph::AudioObject(), channels(channels), sampleRate(rate), blockSize(blockSize) {
    name = "GraphSink";
}

void dibiff::sink::GraphSink::initialize() {
    for (int i = 0; i < channels; i++) {
        inputs.push_back(std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "GraphSinkInput" + std::to_string(i)));
        ringBuffers.push_back(std::make_shared<RingBuffer<float>>(blockSize * 10));
    }
}

void dibiff::sink::GraphSink::process() {
    for (int i = 0; i < channels; i++) {
        if (!inputs[i]->isConnected()) {
            /// Fill ring buffers with zeros
            std::vector<float> zeros(blockSize, 0.0f);
            ringBuffers[i]->write(zeros.data(), blockSize);
        } else if (inputs[i]->isReady()) {
            auto audioData = inputs[i]->getData();
            int blockSize = inputs[i]->getBlockSize();
            /// Add the audioData to the ring buffers
            ringBuffers[i]->write(audioData->data(), blockSize);
        }
    }
    markProcessed();
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::GraphSink::getInput(int i) {
    return inputs[i];
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::GraphSink::getOutput(int i) {
    return std::weak_ptr<dibiff::graph::AudioOutput>();
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::GraphSink::getReference() {
    return std::weak_ptr<dibiff::graph::AudioReference>();
}

bool dibiff::sink::GraphSink::isFinished() const {
    bool allFinished = true;
    for (int i = 0; i < channels; ++i) {
        if (!inputs[i]->isConnected() || !inputs[i]->isReady() || !inputs[i]->isFinished()) {
            allFinished = false;
            break;
        }
    }
    return allFinished && processed;
}

bool dibiff::sink::GraphSink::isReadyToProcess() const {
    std::vector<bool> connected(channels, false);
    for (int i = 0; i < channels; ++i) {
        connected[i] = inputs[i]->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < channels; ++i) {
        if (connected[i] && !inputs[i]->isReady()) {
            isReady = false;
            break;
        }
    }
    return isReady && !processed;
}

std::shared_ptr<dibiff::sink::GraphSink> dibiff::sink::GraphSink::create(int channels, int rate, int blockSize) {
    auto instance = std::make_shared<GraphSink>(channels, rate, blockSize);
    instance->initialize();
    return instance;
}