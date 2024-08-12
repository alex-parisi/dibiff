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
        _inputs.push_back(std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "GraphSinkInput" + std::to_string(i)));
        ringBuffers.push_back(std::make_shared<RingBuffer<float>>(blockSize * 10));
    }
}

void dibiff::sink::GraphSink::process() {
    for (int i = 0; i < channels; i++) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        if (!in->isConnected()) {
            /// Fill ring buffers with zeros
            std::vector<float> zeros(blockSize, 0.0f);
            ringBuffers[i]->write(zeros.data(), blockSize);
        } else if (in->isReady()) {
            auto audioData = in->getData();
            int blockSize = in->getBlockSize();
            /// Add the audioData to the ring buffers
            ringBuffers[i]->write(audioData->data(), blockSize);
        }
    }
    markProcessed();
}

bool dibiff::sink::GraphSink::isFinished() const {
    bool allFinished = true;
    for (int i = 0; i < channels; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        if (!in->isConnected() || !in->isReady() || !in->isFinished()) {
            allFinished = false;
            break;
        }
    }
    return allFinished && processed;
}

bool dibiff::sink::GraphSink::isReadyToProcess() const {
    std::vector<bool> connected(channels, false);
    for (int i = 0; i < channels; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        connected[i] = in->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < channels; ++i) {
        auto in = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(_inputs[i]);
        if (connected[i] && !in->isReady()) {
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