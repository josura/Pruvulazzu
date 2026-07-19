#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.hxx"
#include "DSP/FileLoader.hxx"
#include "UI/WaveformComponent.hxx"

/**
 * @brief The main editor class for the Pruvulazzu audio plugin.
 * This class is responsible for the graphical user interface of the plugin, including parameter controls and waveform visualization. 
 * It inherits from juce::AudioProcessorEditor, juce::FileDragAndDropTarget, and juce::Timer to handle audio processing, file drag-and-drop functionality, and periodic updates respectively.
 * It also manages the synchronization between the UI controls and the underlying audio processor parameters using juce::AudioProcessorValueTreeState::SliderAttachment.
 * The included sliders for now are:
 * - Density: Controls the density of the grains (how many grains are played per second).
 * - Size: Controls the size of the grains.
 * - Start: Controls the start position of the sample playback(where to draw the grains from, along with the end).
 * - End: Controls the end position of the sample playback.
 */
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

    juce::Label densityLabel;
    juce::Label sizeLabel;
    juce::Label startLabel;
    juce::Label endLabel;

    juce::Slider densitySlider;
    juce::Slider sizeSlider;
    juce::Slider startSlider;
    juce::Slider endSlider;

    // These "Attachments" automatically sync the UI knobs to the processor parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> densityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> startAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> endAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PruvulazzuAudioProcessorEditor)
};