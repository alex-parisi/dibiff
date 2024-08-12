/// SampleGenerator.h

#pragma once

#include "generator.h"
#include "../graph/graph.h"
#include "../inc/Eigen/Dense"

class dibiff::generator::SampleGenerator : public dibiff::generator::Generator {
    public:
        std::shared_ptr<dibiff::graph::MidiInput> input;
        std::vector<std::shared_ptr<dibiff::graph::AudioOutput>> outputs;
        SampleGenerator(std::string filename, int blockSize, int sampleRate);
        void initialize() override;
        void process() override;
        void reset() override;
        void clear() override {}
        bool isReadyToProcess() const override;
        bool isFinished() const override;
        static std::shared_ptr<SampleGenerator> create(std::string filename, int blockSize, int sampleRate);
    private:
        std::string filename;
        int blockSize;
        int sampleRate;
        std::vector<Eigen::VectorXf> samples;
        int totalSamples;
        int currentSample;
        void loadSamples(std::string filename);
        int hasNoteOnNoteOff(std::vector<unsigned char> message);
};