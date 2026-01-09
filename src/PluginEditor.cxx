#include "PluginEditor.hxx"

PruvulazzuAudioProcessorEditor::PruvulazzuAudioProcessorEditor(PruvulazzuAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p),
      waveformVisualizer(processor.getFormatManager())
{
    setSize(600, 400);
    addAndMakeVisible(testButton);
    addAndMakeVisible(waveformVisualizer);

    // When clicked, tell the processor to trigger a note
    testButton.onClick = [this] { 
        processor.triggerTestNote(); 
    };
    
}

PruvulazzuAudioProcessorEditor::~PruvulazzuAudioProcessorEditor() {}

void PruvulazzuAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PruvulazzuAudioProcessorEditor::resized() {
    // Define the main boundary with some padding
    auto bounds = getLocalBounds().reduced(20);
    
    // Split the vertical space: 
    // Bottom 50 pixels for buttons/controls
    auto controlArea = bounds.removeFromBottom(50);
    
    // The rest of the top area goes to the waveform
    waveformVisualizer.setBounds(bounds);
    
    // Position the test button in the bottom right of the control area
    // We center it vertically within that 50px slice
    testButton.setBounds(controlArea.removeFromRight(120).withSizeKeepingCentre(100, 30));
}

bool PruvulazzuAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray&) { return true; }

void PruvulazzuAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int) {
    juce::File file(files[0]);
    waveformVisualizer.setFile(file);
    auto newBuffer = loader.loadFile(file);
    processor.getSampleManager().updateCurrentBuffer(newBuffer);
}