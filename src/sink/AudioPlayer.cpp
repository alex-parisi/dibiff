/// AudioPlayer.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "AudioPlayer.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

dibiff::sink::AudioPlayer::AudioPlayer(int rate, int blockSize)
: dibiff::graph::AudioObject(), sampleRate(rate), blockSize(blockSize) {
    name = "AudioPlayer";
}

void dibiff::sink::AudioPlayer::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "AudioPlayerInput");
    ringBuffer = std::make_unique<RingBuffer>(blockSize * 10);
}

void dibiff::sink::AudioPlayer::process() {
    if (input->isReady()) {
        auto audioData = input->getData();
        int blockSize = input->getBlockSize();
        /// Add the audioData to the ring buffer
        ringBuffer->write(audioData->data(), blockSize);
        markProcessed();
    }
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::AudioPlayer::getInput(int i) {
    return input;
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::AudioPlayer::getOutput(int i) {
    return std::weak_ptr<dibiff::graph::AudioOutput>();
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::AudioPlayer::getReference() {
    return std::weak_ptr<dibiff::graph::AudioReference>();
}

bool dibiff::sink::AudioPlayer::isFinished() const {
    return input->isConnected() && input->isReady() && input->isFinished() && processed;
}

bool dibiff::sink::AudioPlayer::isReadyToProcess() const {
    return input->isConnected() && input->isReady() && !processed;
}

std::shared_ptr<dibiff::sink::AudioPlayer> dibiff::sink::AudioPlayer::create(int rate, int blockSize) {
    auto instance = std::make_shared<AudioPlayer>(rate, blockSize);
    instance->initialize();
    return instance;
}