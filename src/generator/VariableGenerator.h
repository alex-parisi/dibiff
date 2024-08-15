/// VariableGenerator.h

#pragma once

#include "generator.h"
#include "../graph/graph.h"

class dibiff::generator::VariableGenerator : public dibiff::generator::Generator {
    public:
        enum class GeneratorState {
            Sine,
            Square,
            Triangle,
        };
        std::shared_ptr<dibiff::graph::MidiInput> input;
        std::shared_ptr<dibiff::graph::AudioOutput> output;
        VariableGenerator(int blockSize, int sampleRate, float& state, float dutyCycle = 0.5f, float frequency = 1000.0f, int totalSamples = -1);
        void initialize() override;
        void process() override;
        void reset() override;
        void clear() override {}
        bool isReadyToProcess() const override;
        bool isFinished() const override;
        static std::shared_ptr<VariableGenerator> create(int blockSize, float sampleRate, float& state, float dutyCycle = 0.5f, float frequency = 1000.0f, int totalSamples = -1);
        void generateSine(float freq);
        void generateSquare(float freq);
        void generateTriangle(float freq);
    private:
        int blockSize;
        int sampleRate;
        float frequency;
        int totalSamples;
        int currentSample;
        float phase;
        float dutyCycle;
        float& state;
};