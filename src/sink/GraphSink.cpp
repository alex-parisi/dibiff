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
        auto in = std::make_unique<dibiff::graph::AudioInput>(dibiff::graph::AudioInput(this, "GraphSinkInput" + std::to_string(i)));
        _inputs.emplace_back(std::move(in));
        ringBuffers.push_back(std::make_unique<RingBuffer<float>>(blockSize * 10));
    }
}

void dibiff::sink::GraphSink::process() {
    for (int i = 0; i < channels; i++) {
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
        if (!in->isConnected()) {
            /// Fill ring buffers with zeros
            std::vector<float> zeros(blockSize, 0.0f);
            ringBuffers[i]->write(zeros, blockSize);
        } else if (in->isReady()) {
            const std::vector<float>& audioData = in->getData();
            const int blockSize = in->getBlockSize();
            /// Add the audioData to the ring buffers
            ringBuffers[i]->write(audioData, blockSize);
        }
    }
    markProcessed();
}

bool dibiff::sink::GraphSink::isFinished() const {
    bool allFinished = true;
    for (int i = 0; i < channels; ++i) {
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
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
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
        connected[i] = in->isConnected();
    }
    bool isReady = true;
    for (int i = 0; i < channels; ++i) {
        auto in = static_cast<dibiff::graph::AudioInput*>(_inputs[i].get());
        if (connected[i] && !in->isReady()) {
            isReady = false;
            break;
        }
    }
    return isReady && !processed;
}

std::unique_ptr<dibiff::sink::GraphSink> dibiff::sink::GraphSink::create(int channels, int rate, int blockSize) {
    auto instance = std::make_unique<GraphSink>(channels, rate, blockSize);
    instance->initialize();
    return std::move(instance);
}