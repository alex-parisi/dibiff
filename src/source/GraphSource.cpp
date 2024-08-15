/// GraphSource.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "GraphSource.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

dibiff::source::GraphSource::GraphSource(int channels, int rate, int blockSize)
: dibiff::graph::AudioObject(), channels(channels), sampleRate(rate), blockSize(blockSize) {
    name = "GraphSource";
}

void dibiff::source::GraphSource::initialize() {
    for (int i = 0; i < channels; i++) {
        _outputs.push_back(std::make_shared<dibiff::graph::AudioOutput>(shared_from_this(), "GraphSourceOutput" + std::to_string(i)));
        ringBuffers.push_back(std::make_shared<RingBuffer<float>>(blockSize * 10));
    }
}

void dibiff::source::GraphSource::process() {
    for (int i = 0; i < channels; i++) {
        auto out = std::dynamic_pointer_cast<dibiff::graph::AudioOutput>(_outputs[i]);
        /// Read the data from the ring buffer
        if (ringBuffers[i]->available() > 0) {
            std::vector<float> ringData(blockSize);
            ringBuffers[i]->read(ringData.data(), blockSize);
            out->setData(ringData, blockSize);
        } else {
            std::vector<float> ringData(blockSize, 0.0f);
            out->setData(ringData, blockSize);
        }
    }
    markProcessed();
}

bool dibiff::source::GraphSource::isFinished() const {
    return false;
}

bool dibiff::source::GraphSource::isReadyToProcess() const {
    return !processed;
}

std::shared_ptr<dibiff::source::GraphSource> dibiff::source::GraphSource::create(int channels, int rate, int blockSize) {
    auto instance = std::make_shared<GraphSource>(channels, rate, blockSize);
    instance->initialize();
    return instance;
}