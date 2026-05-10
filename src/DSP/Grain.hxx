// DSP/Grain.hxx
#pragma once
#include "Envelopes.hxx"

class Grain {
public:
    Grain() = default;

    // Returns normalized position (0.0 to 1.0) within the source buffer
    float getNormalizedPosition(int sourceLength) const {
        if (!active || sourceLength <= 0) return -1.0f;
        // Calculate the current read position in the source buffer
        return static_cast<float>((startSample + currentSample) % sourceLength) / sourceLength;
    }

    void init(int start, int len, float p, Envelope* env) {
        startSample = start;
        duration = len;
        pan = p;
        currentSample = 0;
        active = true;
        currentEnvelope = env;
    }

    // Process one sample and return the result
    // The core DSP logic for a single grain
    float process(const float* sourceData, int sourceLength) {
        // 1. Safety Check
        if (!active || sourceLength <= 0) return 0.0f;

        // 2. Read the raw sample from the circular buffer
        int readPos = (startSample + currentSample) % sourceLength;
        float rawSample = sourceData[readPos];

        // 3. Calculate the Window Function (Hann Window)
        // A Hann window equation is: 0.5 * (1 - cos(2 * PI * n / N))
        // Where 'n' is current position, and 'N' is total length.
        float windowPhase = static_cast<float>(currentSample) / static_cast<float>(duration - 1);
        float windowLevel = 0.5f * (1.0f - std::cos(juce::MathConstants<float>::twoPi * windowPhase));

        // 4. Apply the window to the sample
        float outputSample = rawSample * windowLevel;

        // 5. Advance the playhead for this grain
        currentSample++;
        
        // 6. Check if the grain has finished playing
        if (currentSample >= duration) {
            active = false;
        }

        return outputSample;
    }

    bool isActive() const { return active; }
    float getPan() const { return pan; }

private:
    int startSample = 0;
    int duration = 0;
    int currentSample = 0;
    int currentSampleIndex = 0; // The absolute index in the source buffer
    bool active = false;
    float pan = 0.5f;
    Envelope* currentEnvelope = nullptr; // Pointer to the envelope controlling this grain, unused since we are using a fixed Hann window
};