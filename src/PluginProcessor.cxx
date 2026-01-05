#include "PluginProcessor.hxx"
#include "PluginEditor.hxx"

AeolusAudioProcessor::AeolusAudioProcessor() 
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    formatManager.registerBasicFormats();
}

AeolusAudioProcessor::~AeolusAudioProcessor() {}

void AeolusAudioProcessor::prepareToPlay (double, int) { playhead = 0; }
void AeolusAudioProcessor::releaseResources() {}

void AeolusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Check if a test note has been triggered from the UI
    if (noteTriggered.exchange(false))
    {
        // Inject a MIDI Note On (Note 60, Velocity 1.0) into the buffer
        midiMessages.addEvent(juce::MidiMessage::noteOn(1, 60, 1.0f), 0);
    }

    // Handle MIDI
    for (const auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            playhead = 0;
            isNoteActive = true;
        } else if (msg.isNoteOff()) {
            isNoteActive = false;
        }
    }

    buffer.clear();
    if (!isNoteActive) return;

    auto currentData = sampleManager.getCurrentBuffer();
    if (currentData == nullptr) return;

    const auto& sampleBuffer = currentData->getBuffer();
    auto* leftOut = buffer.getWritePointer(0);
    auto* rightOut = buffer.getWritePointer(1);
    const auto* sampleIn = sampleBuffer.getReadPointer(0);

    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        if (playhead < sampleBuffer.getNumSamples()) {
            float sample = sampleIn[playhead];
            leftOut[i] += sample;
            if (buffer.getNumChannels() > 1) rightOut[i] += sample;
            playhead++;
        }
    }
}

juce::AudioProcessorEditor* AeolusAudioProcessor::createEditor() {
    return new AeolusAudioProcessorEditor(*this);
}

// Mandatory JUCE requirement: this function must be implemented
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AeolusAudioProcessor();
}