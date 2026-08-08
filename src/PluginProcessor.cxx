/**
 * Pruvulazzu - A JUCE-based granular synthesizer plugin
 * Copyright (C) 2026  Giorgio Locicero
 * @brief This file contains the implementation of the main audio processor class for the Pruvulazzu plugin.
 */
#include "PluginProcessor.hxx"
#include "PluginEditor.hxx"

PruvulazzuAudioProcessor::PruvulazzuAudioProcessor() 
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", {
          std::make_unique<juce::AudioParameterFloat> ("density", "Density (Hz)", 1.0f, 100.0f, 20.0f),
          std::make_unique<juce::AudioParameterFloat> ("size", "Grain Size (ms)", 10.0f, 1000.0f, 150.0f),
          // Normalized start/end parameters (0.0 = beginning of file, 1.0 = end)
          std::make_unique<juce::AudioParameterFloat> ("start", "Start", 0.0f, 1.0f, 0.0f),
          std::make_unique<juce::AudioParameterFloat> ("end", "End", 0.0f, 1.0f, 1.0f)
      })
{
    formatManager.registerBasicFormats();
}

PruvulazzuAudioProcessor::~PruvulazzuAudioProcessor() {}

// void PruvulazzuAudioProcessor::prepareToPlay (double, int) { playhead = 0; }

void PruvulazzuAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    grainEngine.prepare(100); // Allow up to 100 simultaneous grains
    samplesSinceLastGrain = 0;
}

void PruvulazzuAudioProcessor::releaseResources() {}

std::vector<float> PruvulazzuAudioProcessor::getActiveGrainPositions() const  {
    auto currentData = sampleManager.getCurrentBuffer();
    int totalLen = (currentData != nullptr) ? currentData->getBuffer().getNumSamples() : 0;
    return grainEngine.getActivePositions(totalLen);
}


void PruvulazzuAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    buffer.clear();
    auto currentData = sampleManager.getCurrentBuffer();
    if (currentData == nullptr) return;

    int totalSamples = currentData->getBuffer().getNumSamples();

    // Test button logic: One Macro Grain
    if (noteTriggered.exchange(false)) {
        // Play the entire sample as one singular grain
        grainEngine.triggerGrain(0, totalSamples, 0.5f, activeEnvelope.get());
    }

    // Midi logic: Turn granular cloud on/off
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) isNoteActive = true;
        else if (msg.isNoteOff()) isNoteActive = false;
    }

    // Granular scheduler logic: Trigger grains based on density parameter
    if (isNoteActive) {
        float density = apvts.getRawParameterValue("density")->load();
        
        // Density is grains per second. Interval = SampleRate / Density
        int intervalSamples = static_cast<int>(getSampleRate() / density);
        
        samplesSinceLastGrain += buffer.getNumSamples();
        if (samplesSinceLastGrain >= intervalSamples) {
            float sizeMs = apvts.getRawParameterValue("size")->load();
            int len = static_cast<int>((sizeMs / 1000.0f) * getSampleRate());
            
            // get normalized start and end positions
            float startNorm = apvts.getRawParameterValue("start")->load();
            float endNorm = apvts.getRawParameterValue("end")->load();
            if (startNorm > endNorm) std::swap(startNorm, endNorm); // Ensure start <= end

            // Compute actual sample positions based on normalized values
            int startSample = static_cast<int>(startNorm * totalSamples);
            int endSample = static_cast<int>(endNorm * totalSamples);

            // Ensure the grain fully fits inside the end boundary
            int maxStart = endSample - len;
            if (maxStart < startSample) maxStart = startSample; // Fallback if size is larger than the window
            
            int spawnPos = startSample;
            if (maxStart > startSample) {
                spawnPos = startSample + juce::Random::getSystemRandom().nextInt(maxStart - startSample + 1);
            }
            
            grainEngine.triggerGrain(spawnPos, len, 0.5f, activeEnvelope.get());
            samplesSinceLastGrain -= intervalSamples; // Subtract instead of reset to 0 to keep precise timing
        }
    }

    grainEngine.process(buffer, currentData);
}

juce::AudioProcessorEditor* PruvulazzuAudioProcessor::createEditor() {
    return new PruvulazzuAudioProcessorEditor(*this);
}

// Mandatory JUCE requirement: this function must be implemented
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PruvulazzuAudioProcessor();
}