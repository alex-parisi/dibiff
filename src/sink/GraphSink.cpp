/// AudioPlayer.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "GraphSink.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

dibiff::sink::GraphSink::GraphSink(int rate, int blockSize)
: dibiff::graph::AudioObject(), sampleRate(rate), blockSize(blockSize) {
    name = "GraphSink";
}

void dibiff::sink::GraphSink::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "GraphSinkInput");
    ringBuffer = std::make_shared<RingBuffer<float>>(blockSize * 10);
}

void dibiff::sink::GraphSink::process() {
    if (!input->isConnected()) {
        /// Fill ring buffer with zeros
        std::vector<float> zeros(blockSize, 0.0f);
        ringBuffer->write(zeros.data(), blockSize);
        markProcessed();
    } else if (input->isReady()) {
        auto audioData = input->getData();
        int blockSize = input->getBlockSize();
        /// Add the audioData to the ring buffer
        ringBuffer->write(audioData->data(), blockSize);
        markProcessed();
    }
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::GraphSink::getInput(int i) {
    return input;
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::GraphSink::getOutput(int i) {
    return std::weak_ptr<dibiff::graph::AudioOutput>();
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::GraphSink::getReference() {
    return std::weak_ptr<dibiff::graph::AudioReference>();
}

bool dibiff::sink::GraphSink::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}

bool dibiff::sink::GraphSink::isReadyToProcess() const {
    if (!input->isConnected()) {
        return true;
    }
    return input->isReady() && !processed;
}

std::shared_ptr<dibiff::sink::GraphSink> dibiff::sink::GraphSink::create(int rate, int blockSize) {
    auto instance = std::make_shared<GraphSink>(rate, blockSize);
    instance->initialize();
    return instance;
}