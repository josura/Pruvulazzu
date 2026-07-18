#pragma once
#include <vector>
#include "Grain.hxx"
#include "SampleBufferData.hxx"

class GrainEngine {
public:
    GrainEngine() = default;

    void prepare(int maxGrains) {
        grainPool.resize(maxGrains);
    }

    void triggerGrain(int start, int length, float pan, Envelope* env) {
        for (auto& grain : grainPool) {
            if (!grain.isActive()) {
                grain.init(start, length, pan, env);
                return;
            }
        }
    }

    // Snapshot of all active grain positions (0.0 to 1.0)
    std::vector<float> getActivePositions(int totalSampleLength) const {
        std::vector<float> positions;
        if (totalSampleLength <= 0) return positions; // Avoid division by zero, return empty vector
        
        for (const auto& grain : grainPool) {
            if (grain.isActive()) {
                // We need to add getAbsolutePosition() to the Grain class
                positions.push_back(grain.getNormalizedPosition(totalSampleLength));
            }
        }
        return positions;
    }

    void process(juce::AudioBuffer<float>& outputBuffer, const SampleBufferData::Ptr& sampleData) {
        if (sampleData == nullptr) return;
        const auto& sourceBuffer = sampleData->getBuffer();
        const float* sourceRaw = sourceBuffer.getReadPointer(0);
        int sourceLen = sourceBuffer.getNumSamples();

        for (int s = 0; s < outputBuffer.getNumSamples(); ++s) {
            float leftSum = 0.0f, rightSum = 0.0f;
            for (auto& grain : grainPool) {
                if (grain.isActive()) {
                    float sample = grain.process(sourceRaw, sourceLen);
                    float p = grain.getPan();
                    leftSum += sample * std::sqrt(1.0f - p);
                    rightSum += sample * std::sqrt(p);
                }
            }
            outputBuffer.addSample(0, s, leftSum);
            outputBuffer.addSample(1, s, rightSum);
        }
    }

    // void process(juce::AudioBuffer<float>& outputBuffer, const SampleBufferData::Ptr& sampleData) {
    //     if (sampleData == nullptr) return;

    //     const auto& sourceBuffer = sampleData->getBuffer();
    //     const float* sourceRaw = sourceBuffer.getReadPointer(0);
    //     int sourceLen = sourceBuffer.getNumSamples();

    //     auto* leftOut = outputBuffer.getWritePointer(0);
    //     auto* rightOut = outputBuffer.getWritePointer(1);

    //     for (int s = 0; s < outputBuffer.getNumSamples(); ++s) {
    //         for (auto& grain : grainPool) {
    //             if (grain.isActive()) {
    //                 float sample = grain.process(sourceRaw, sourceLen);  // Process grain sample
    //                 float p = grain.getPan();  // Get pan value
    //                 leftOut[s] += sample * std::sqrt(1.0f - p);  // Simple equal-power panning
    //                 rightOut[s] += sample * std::sqrt(p);
    //             }
    //         }
    //     }
    // }

private:
    std::vector<Grain> grainPool;
};