#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.hxx"
#include "DSP/FileLoader.hxx"
#include "UI/WaveformComponent.hxx"

class PruvulazzuAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::FileDragAndDropTarget,
                                   public juce::Timer
{
public:
    PruvulazzuAudioProcessorEditor(PruvulazzuAudioProcessor&);
    ~PruvulazzuAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray&) override;
    void filesDropped(const juce::StringArray&, int, int) override;
    /**
     * @brief Timer callback to update playhead positions
     */
    void timerCallback() override;

private:
    juce::TextButton testButton { "Test" };
    PruvulazzuAudioProcessor& processor;
    WaveformComponent waveformVisualizer;
    FileLoader loader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PruvulazzuAudioProcessorEditor)
};