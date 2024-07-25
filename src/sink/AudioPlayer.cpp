/// AudioPlayer.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "AudioPlayer.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

std::string dibiff::sink::AudioPlayer::getName() const {
    return "AudioPlayer";
}

dibiff::sink::AudioPlayer::AudioPlayer(int rate, int blockSize)
    : sampleRate(rate), blockSize(blockSize) {}

void dibiff::sink::AudioPlayer::initialize() {
    input = std::make_shared<dibiff::graph::AudioInput>(shared_from_this(), "AudioPlayerInput");

    ringBuffer = std::make_unique<RingBuffer>(blockSize * 10);

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_f32;
    config.playback.channels = 1;
    config.sampleRate = sampleRate;
    config.dataCallback = dataCallback;
    config.pUserData = this;
    config.periodSizeInFrames = blockSize;

    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio device." << std::endl;
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "Failed to start audio device." << std::endl;
        ma_device_uninit(&device);
    }
}

dibiff::sink::AudioPlayer::~AudioPlayer() {
    ma_device_uninit(&device);
}

void dibiff::sink::AudioPlayer::process() {
    if (input->isReady()) {
        auto audioData = input->getData();
        int blockSize = input->getBlockSize();
        ringBuffer->write(audioData->data(), blockSize);
        markProcessed();
    }
}

void dibiff::sink::AudioPlayer::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioPlayer* audioPlayer = static_cast<AudioPlayer*>(pDevice->pUserData);
    if (audioPlayer == nullptr || audioPlayer->ringBuffer == nullptr) return;
    float* outputBuffer = static_cast<float*>(pOutput);
    size_t samplesAvailable = audioPlayer->ringBuffer->available();
    audioPlayer->ringBuffer->read(outputBuffer, samplesAvailable);

    // Signal that the callback has finished processing
    std::lock_guard<std::mutex> lock(audioPlayer->cv_mtx);
    audioPlayer->cv.notify_one();
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::AudioPlayer::getInput(int i) {
    return input;
}

std::weak_ptr<dibiff::graph::AudioConnectionPoint> dibiff::sink::AudioPlayer::getOutput() {
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