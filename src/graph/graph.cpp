/// graph.cpp

#include "graph.h"

#include <queue>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <iostream>

#include "../generator/generator.h"
#include "../sink/AudioPlayer.h"

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
 * @brief GLFW Error Callback
 */
void dibiff::graph::AudioObject::glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
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
    double lastRenderTime = 0.0;
    const double renderFPS = 60.0;
    const double renderInterval = 1.0 / renderFPS;
    // Initialize GLFW
    glfwSetErrorCallback(dibiff::graph::AudioObject::glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }
    // Set GLFW context version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui + GLFW + AudioObject Example", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    // Load OpenGL functions using glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL context\n";
        return;
    }
    // Set up ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // Initialize ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(currentTime.time_since_epoch()).count();
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
            std::shared_ptr<dibiff::sink::AudioPlayer> audioPlayer = nullptr;
            for (auto& obj : objects) {
                if (auto o = std::dynamic_pointer_cast<dibiff::sink::AudioPlayer>(obj)) {
                    audioPlayer = std::dynamic_pointer_cast<dibiff::sink::AudioPlayer>(obj);
                    break;
                }
            }
            if (audioPlayer == nullptr) {
                std::cerr << "Warning: Audio player not found in real-time mode.\n";
                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = endTime - startTime;
                double sleepTime = blockDuration - elapsed.count();
                if (sleepTime > 0) {
                    std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
                }
            } else {
                /// Wait for the audio player to finish processing
                std::unique_lock<std::mutex> lock(audioPlayer->cv_mtx);
                audioPlayer->cv.wait(lock);
            }
        }
        if ((elapsedTime - lastRenderTime) >= renderInterval) {
            lastRenderTime = elapsedTime;
            glfwPollEvents();
            // Start the ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            // Render ImGui UI for each AudioObject
            for (auto& obj : objects) {
                obj->RenderImGui();
            }
            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
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