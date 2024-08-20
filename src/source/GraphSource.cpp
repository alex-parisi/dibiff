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
        auto o = std::make_unique<dibiff::graph::AudioOutput>(dibiff::graph::AudioOutput(this, "GraphSourceOutput" + std::to_string(i)));
        _outputs.emplace_back(std::move(o));
        ringBuffers.push_back(std::make_unique<RingBuffer<float>>(blockSize * 10));
    }
}

void dibiff::source::GraphSource::process() {
    for (int i = 0; i < channels; i++) {
        auto out = static_cast<dibiff::graph::AudioOutput*>(_outputs[i].get());
        /// Read the data from the ring buffer
        if (ringBuffers[i]->available() > 0) {
            std::vector<float> ringData = ringBuffers[i]->read(blockSize);
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

std::unique_ptr<dibiff::source::GraphSource> dibiff::source::GraphSource::create(int channels, int rate, int blockSize) {
    auto instance = std::make_unique<GraphSource>(channels, rate, blockSize);
    instance->initialize();
    return std::move(instance);
}