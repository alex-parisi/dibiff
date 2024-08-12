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
class dibiff::graph::AudioInput : public dibiff::graph::AudioConnectionPoint, public std::enable_shared_from_this<AudioInput> {
    public:
        std::weak_ptr<dibiff::graph::AudioOutput> connectedOutput;
        std::weak_ptr<dibiff::graph::AudioObject> parent;
        AudioInput(std::weak_ptr<dibiff::graph::AudioObject> parent, std::string name) 
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        void connect(std::weak_ptr<dibiff::graph::AudioOutput> output);
        void disconnect();
        bool isConnected() override;
        bool isReady() const;
        bool isFinished() const;
        std::optional<std::vector<float>> getData() const;
        int getBlockSize() const;
};
/**
 * @brief MIDI Input Connection Point
 * @details A MIDI input connection point that can be used to connect MIDI objects
 */
class dibiff::graph::MidiInput : public dibiff::graph::AudioConnectionPoint, public std::enable_shared_from_this<MidiInput> {
    public:
        std::weak_ptr<dibiff::graph::MidiOutput> connectedOutput;
        std::weak_ptr<dibiff::graph::AudioObject> parent;
        MidiInput(std::weak_ptr<dibiff::graph::AudioObject> parent, std::string name) 
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        void connect(std::weak_ptr<dibiff::graph::MidiOutput> output);
        void disconnect();
        bool isConnected() override;
        bool isReady() const;
        bool isFinished() const;
        std::optional<std::vector<std::vector<unsigned char>>> getData() const;
        int getBlockSize() const;
};
/**
 * @brief Audio Output Connection Point
 * @details An audio output connection point that can be used to connect audio objects
 */
class dibiff::graph::AudioOutput : public dibiff::graph::AudioConnectionPoint, public std::enable_shared_from_this<AudioOutput> {
    public:
        std::weak_ptr<dibiff::graph::AudioObject> parent;
        std::vector<std::weak_ptr<dibiff::graph::AudioInput>> connectedInputs = {};
        std::vector<float> data;
        int blockSize;
        AudioOutput(std::weak_ptr<dibiff::graph::AudioObject> parent, std::string name)
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        bool isProcessed() const;
        bool isFinished() const;
        void setData(std::vector<float>& audioData, int N);
        std::vector<float> getData() const;
        int getBlockSize() const;
        void connect(std::weak_ptr<dibiff::graph::AudioInput> inChannel);
        void disconnect(std::weak_ptr<dibiff::graph::AudioInput> inChannel);
        void disconnect();
        bool isConnected() override;
};
/**
 * @brief MIDI Output Connection Point
 * @details A MIDI output connection point that can be used to connect MIDI objects
 */
class dibiff::graph::MidiOutput : public dibiff::graph::AudioConnectionPoint, public std::enable_shared_from_this<dibiff::graph::MidiOutput> {
    public:
        std::weak_ptr<dibiff::graph::AudioObject> parent;
        std::vector<std::weak_ptr<dibiff::graph::MidiInput>> connectedInputs = {};
        std::vector<std::vector<unsigned char>> data;
        int blockSize;
        MidiOutput(std::weak_ptr<dibiff::graph::AudioObject> parent, std::string name)
        : dibiff::graph::AudioConnectionPoint(name), 
          parent(parent) {};
        bool isProcessed() const;
        bool isFinished() const;
        void setData(std::vector<std::vector<unsigned char>>& midiData, int N);
        std::vector<std::vector<unsigned char>> getData() const;
        int getBlockSize() const;
        void connect(std::weak_ptr<dibiff::graph::MidiInput> inChannel);
        void disconnect(std::weak_ptr<dibiff::graph::MidiInput> inChannel);
        void disconnect();
        bool isConnected() override;
};
/**
 * @brief Generic Audio Object
 * @details A generic audio object that can be used in audio processing applications
 */
class dibiff::graph::AudioObject : public std::enable_shared_from_this<AudioObject> {
    public:
        /// TODO: Remove underscore from variable name
        std::vector<std::shared_ptr<dibiff::graph::AudioConnectionPoint>> _inputs;
        std::vector<std::shared_ptr<dibiff::graph::AudioConnectionPoint>> _outputs;
        std::string name = "AudioObject";
        virtual void reset() = 0;
        virtual void clear() = 0;
        virtual void process() = 0;
        virtual bool isReadyToProcess() const = 0;
        /// TODO: Remove these two functions: 
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) { return _inputs[i]; }
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) { return _outputs[i]; }
        virtual bool isFinished() const = 0;
        virtual void initialize() = 0;
        std::string getName() { return name; }
        void setName(std::string name) { this->name = name; }
        virtual ~AudioObject() {};
        void markProcessed(bool processed = true) { this->processed = processed; }
        bool isProcessed() const { return processed; }
        void disconnectAll() {
            for (auto& input : _inputs) {
                auto i = std::dynamic_pointer_cast<dibiff::graph::AudioInput>(input);
                if (i) {
                    if (i->isConnected()) {
                        if (auto ii = i->connectedOutput.lock()) {
                            ii->disconnect(i);
                        }
                    }
                } else {
                    auto mi = std::dynamic_pointer_cast<dibiff::graph::MidiInput>(input);
                    if (mi) {
                        if (mi->isConnected()) {
                            if (auto mmi = mi->connectedOutput.lock()) {
                                mmi->disconnect(mi);
                            }
                        }
                    }
                }
            }
            for (auto& output : _outputs) {
                auto o = std::dynamic_pointer_cast<dibiff::graph::AudioOutput>(output);
                if (o) {
                    if (!o->connectedInputs.empty()) {
                        for (auto& input : o->connectedInputs) {
                            if (auto oi = input.lock()) {
                                o->disconnect(oi);
                            }
                        }
                    }
                } else {
                    auto mo = std::dynamic_pointer_cast<dibiff::graph::MidiOutput>(output);
                    if (mo) {
                        if (!mo->connectedInputs.empty()) {
                            for (auto& input : mo->connectedInputs) {
                                if (auto mmo = input.lock()) {
                                    mo->disconnect(mmo);
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
class dibiff::graph::AudioCompositeObject : public std::enable_shared_from_this<AudioCompositeObject> {
    public:
        virtual std::weak_ptr<dibiff::graph::AudioConnectionPoint> getInput(int i = 0) = 0;
        virtual std::weak_ptr<dibiff::graph::AudioConnectionPoint> getOutput(int i = 0) = 0;
        virtual std::weak_ptr<dibiff::graph::AudioConnectionPoint> getReference() = 0;
        virtual void initialize() = 0;
        virtual std::string getName() const = 0;
        std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects;
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
        std::shared_ptr<dibiff::graph::AudioObject> add(std::shared_ptr<dibiff::graph::AudioObject> obj);
        std::shared_ptr<dibiff::graph::AudioCompositeObject> add(std::shared_ptr<dibiff::graph::AudioCompositeObject> obj);
        void remove(std::shared_ptr<dibiff::graph::AudioObject> obj);
        void remove(std::shared_ptr<dibiff::graph::AudioCompositeObject> obj);
        void run(bool realTime = false, int sampleRate = 48000, int blockSize = 512);
        void tick();
        static void connect(std::weak_ptr<dibiff::graph::AudioOutput> outChannel, std::weak_ptr<dibiff::graph::AudioInput> inChannel);
        static void connect(std::weak_ptr<dibiff::graph::AudioInput> inChannel, std::weak_ptr<dibiff::graph::AudioOutput> outChannel);
        static void connect(std::weak_ptr<dibiff::graph::AudioConnectionPoint> pt1, std::weak_ptr<dibiff::graph::AudioConnectionPoint> pt2);
        static void disconnect(std::weak_ptr<dibiff::graph::AudioOutput> outChannel, std::weak_ptr<dibiff::graph::AudioInput> inChannel);
        static void disconnect(std::weak_ptr<dibiff::graph::AudioInput> inChannel, std::weak_ptr<dibiff::graph::AudioOutput> outChannel);
        static void disconnect(std::weak_ptr<dibiff::graph::AudioConnectionPoint> pt1, std::weak_ptr<dibiff::graph::AudioConnectionPoint> pt2);
    private:
        std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects;
};