/// graph.h

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <memory>
#include <cmath>
#include <chrono>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <math.h>

/// TODO: Put these in separate files

namespace dibiff {
    /**
     * @brief Graph Namespace
     * @details A namespace containing a set of classes for building audio processing graphs
     */
    namespace graph {
        class AudioObject;
        class AudioCompositeObject;
        class AudioInput;
        class MidiInput;
        class AudioOutput;
        class MidiOutput;
        class AudioConnectionPoint;
        class AudioGraph;
    }
}
/**
 * @brief Audio Connection Point
 * @details A generic audio connection point that can be used to connect audio objects
 */
class dibiff::graph::AudioConnectionPoint {
    public:
        AudioConnectionPoint(std::string name) : name(name) {};
        virtual ~AudioConnectionPoint() {}
        std::string getName() const { return name; }
        virtual bool isConnected() = 0;
    private:
        std::string name;
};
/**
 * @brief Audio Input Connection Point
 * @details An audio input connection point that can be used to connect audio objects
 */
class dibiff::graph::AudioInput : public dibiff::graph::AudioConnectionPoint {
    public:
        dibiff::graph::AudioOutput* connectedOutput = nullptr;
        dibiff::graph::AudioObject* parent;
        static std::vector<float> empty;
        AudioInput(dibiff::graph::AudioObject* parent, std::string name) 
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        void connect(dibiff::graph::AudioOutput* output);
        void disconnect();
        bool isConnected() override;
        bool isReady() const;
        bool isFinished() const;
        const std::vector<float>& getData() const;
        const int getBlockSize() const;
};
/**
 * @brief MIDI Input Connection Point
 * @details A MIDI input connection point that can be used to connect MIDI objects
 */
class dibiff::graph::MidiInput : public dibiff::graph::AudioConnectionPoint {
    public:
        dibiff::graph::MidiOutput* connectedOutput = nullptr;
        dibiff::graph::AudioObject* parent;
        static std::vector<std::vector<unsigned char>> empty;
        MidiInput(dibiff::graph::AudioObject* parent, std::string name) 
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        void connect(dibiff::graph::MidiOutput* output);
        void disconnect();
        bool isConnected() override;
        bool isReady() const;
        bool isFinished() const;
        const std::vector<std::vector<unsigned char>>& getData() const;
        const int getBlockSize() const;
};
/**
 * @brief Audio Output Connection Point
 * @details An audio output connection point that can be used to connect audio objects
 */
class dibiff::graph::AudioOutput : public dibiff::graph::AudioConnectionPoint {
    public:
        dibiff::graph::AudioObject* parent;
        std::vector<dibiff::graph::AudioInput*> connectedInputs = {};
        std::vector<float> data = {};
        int blockSize;
        AudioOutput(dibiff::graph::AudioObject* parent, std::string name)
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        bool isProcessed() const;
        bool isFinished() const;
        void setData(std::vector<float> audioData, int N);
        const std::vector<float>& getData() const;
        const int getBlockSize() const;
        void connect(dibiff::graph::AudioInput* inChannel);
        void disconnect(dibiff::graph::AudioInput* inChannel);
        void disconnect();
        bool isConnected() override;
};
/**
 * @brief MIDI Output Connection Point
 * @details A MIDI output connection point that can be used to connect MIDI objects
 */
class dibiff::graph::MidiOutput : public dibiff::graph::AudioConnectionPoint {
    public:
        dibiff::graph::AudioObject* parent;
        std::vector<dibiff::graph::MidiInput*> connectedInputs = {};
        std::vector<std::vector<unsigned char>> data = {};
        int blockSize;
        MidiOutput(dibiff::graph::AudioObject* parent, std::string name)
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        bool isProcessed() const;
        bool isFinished() const;
        void setData(std::vector<std::vector<unsigned char>> midiData, int N);
        const std::vector<std::vector<unsigned char>>& getData() const;
        const int getBlockSize() const;
        void connect(dibiff::graph::MidiInput* inChannel);
        void disconnect(dibiff::graph::MidiInput* inChannel);
        void disconnect();
        bool isConnected() override;
};
/**
 * @brief Generic Audio Object
 * @details A generic audio object that can be used in audio processing applications
 */
class dibiff::graph::AudioObject {
    public:
        /// TODO: Remove underscore from variable name
        std::vector<std::unique_ptr<dibiff::graph::AudioConnectionPoint>> _inputs;
        std::vector<std::unique_ptr<dibiff::graph::AudioConnectionPoint>> _outputs;
        std::string name = "AudioObject";
        virtual void reset() = 0;
        virtual void clear() = 0;
        virtual void process() = 0;
        virtual bool isReadyToProcess() const = 0;
        /// TODO: Remove these two functions: 
        dibiff::graph::AudioConnectionPoint* getInput(int i = 0) { return _inputs[i].get(); }
        dibiff::graph::AudioConnectionPoint* getOutput(int i = 0) { return _outputs[i].get(); }
        virtual bool isFinished() const = 0;
        virtual void initialize() = 0;
        std::string getName() { return name; }
        void setName(std::string name) { this->name = name; }
        virtual ~AudioObject() {};
        void markProcessed(bool processed = true) { this->processed = processed; }
        bool isProcessed() const { return processed; }
        void disconnectAll() {
            for (auto& input : _inputs) {
                if (input) {
                    if (auto i = static_cast<dibiff::graph::AudioInput*>(input.get())) {
                        if (i->isConnected()) {
                            if (auto& ii = i->connectedOutput) {
                                ii->disconnect(i);
                            }
                        }
                    } else if (auto mi = static_cast<dibiff::graph::MidiInput*>(input.get())) {
                        if (mi->isConnected()) {
                            if (auto& mmi = mi->connectedOutput) {
                                mmi->disconnect(mi);
                            }
                        }
                    }
                }
            }
            for (auto& output : _outputs) {
                if (output) {
                    if (auto o = static_cast<dibiff::graph::AudioOutput*>(output.get())) {
                        if (!o->connectedInputs.empty()) {
                            for (auto& input : o->connectedInputs) {
                                if (input) {
                                    o->disconnect(input);
                                }
                            }
                        }
                    } else if (auto mo = static_cast<dibiff::graph::MidiOutput*>(output.get())) {
                        if (!mo->connectedInputs.empty()) {
                            for (auto& input : mo->connectedInputs) {
                                if (input) {
                                    mo->disconnect(input);
                                }
                            }
                        }
                    }
                }
            }
        }
    protected:
        bool processed = false;
};
/**
 * @brief Audio Composite Object
 * @details A composite audio object that can be used to create complex audio processing graphs
 */
class dibiff::graph::AudioCompositeObject {
    public:
        virtual dibiff::graph::AudioConnectionPoint* getInput(int i = 0) = 0;
        virtual dibiff::graph::AudioConnectionPoint* getOutput(int i = 0) = 0;
        virtual dibiff::graph::AudioConnectionPoint* getReference() = 0;
        virtual void initialize() = 0;
        virtual std::string getName() const = 0;
        std::vector<std::unique_ptr<dibiff::graph::AudioObject>> objects;
};
/**
 * @brief Audio Graph
 * @details An audio graph is a collection of audio objects that are connected
 * together to form a processing graph. The audio graph processes the audio
 * objects in the correct order to generate the final output.
 */
class dibiff::graph::AudioGraph {
    public:
        AudioGraph() {}
        ~AudioGraph() {}
        dibiff::graph::AudioObject* add(dibiff::graph::AudioObject* obj);
        dibiff::graph::AudioCompositeObject* add(dibiff::graph::AudioCompositeObject* obj);
        void remove(dibiff::graph::AudioObject* obj);
        void remove(dibiff::graph::AudioCompositeObject* obj);
        void tick();
        static void connect(dibiff::graph::AudioOutput* outChannel, dibiff::graph::AudioInput* inChannel);
        static void connect(dibiff::graph::AudioInput* inChannel, dibiff::graph::AudioOutput* outChannel);
        static void connect(dibiff::graph::AudioConnectionPoint* pt1, dibiff::graph::AudioConnectionPoint* pt2);
        static void disconnect(dibiff::graph::AudioOutput* outChannel, dibiff::graph::AudioInput* inChannel);
        static void disconnect(dibiff::graph::AudioInput* inChannel, dibiff::graph::AudioOutput* outChannel);
        static void disconnect(dibiff::graph::AudioConnectionPoint* pt1, dibiff::graph::AudioConnectionPoint* pt2);
    private:
        std::vector<dibiff::graph::AudioObject*> objects;
};