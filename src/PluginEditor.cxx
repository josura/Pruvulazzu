#include "PluginEditor.hxx"

PruvulazzuAudioProcessorEditor::PruvulazzuAudioProcessorEditor(PruvulazzuAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p),
      waveformVisualizer(processor.getFormatManager())
{
    setSize(800, 600);
    addAndMakeVisible(testButton);
    addAndMakeVisible(waveformVisualizer);

    // --- Density Knob & Label ---
    densitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    densitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(densitySlider);
    densityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "density", densitySlider);
    
    // Setup Density Label
    densityLabel.setText("Density", juce::dontSendNotification);
    densityLabel.setJustificationType(juce::Justification::centred);
    densityLabel.attachToComponent(&densitySlider, false); // false = place above the slider
    addAndMakeVisible(densityLabel);

    // --- Size Knob & Label ---
    sizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(sizeSlider);
    sizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "size", sizeSlider);
    
    // Setup Size Label
    sizeLabel.setText("Size", juce::dontSendNotification);
    sizeLabel.setJustificationType(juce::Justification::centred);
    sizeLabel.attachToComponent(&sizeSlider, false); // false = place above the slider
    addAndMakeVisible(sizeLabel);

    // --- Start Knob & Label ---
    startSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    startSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(startSlider);
    startAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "start", startSlider);
    
    startLabel.setText("Start", juce::dontSendNotification);
    startLabel.setJustificationType(juce::Justification::centred);
    startLabel.attachToComponent(&startSlider, false); 
    addAndMakeVisible(startLabel);

    // --- End Knob & Label ---
    endSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    endSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(endSlider);
    endAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "end", endSlider);
    
    endLabel.setText("End", juce::dontSendNotification);
    endLabel.setJustificationType(juce::Justification::centred);
    endLabel.attachToComponent(&endSlider, false); 
    addAndMakeVisible(endLabel);

    testButton.onClick = [this] { processor.triggerTestNote(); };

    startTimerHz(60);
}

PruvulazzuAudioProcessorEditor::~PruvulazzuAudioProcessorEditor() {}

void PruvulazzuAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

// Replace your resized() function:
void PruvulazzuAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(10);
    
    // Bottom 110 pixels for controls
    auto controlArea = bounds.removeFromBottom(110);
    bounds.removeFromBottom(10); // Spacing between waveform and controls
    
    waveformVisualizer.setBounds(bounds);
    
    // Layout controls horizontally
    int knobWidth = 70;
    auto knobArea = controlArea.withTrimmedTop(10).withTrimmedBottom(10); // Add some vertical padding
    densitySlider.setBounds(knobArea.removeFromLeft(knobWidth));
    sizeSlider.setBounds(knobArea.removeFromLeft(knobWidth));
    startSlider.setBounds(knobArea.removeFromLeft(knobWidth));
    endSlider.setBounds(knobArea.removeFromLeft(knobWidth));

    // densitySlider.setBounds(controlArea.removeFromLeft(knobWidth));
    // sizeSlider.setBounds(controlArea.removeFromLeft(knobWidth));
    
    testButton.setBounds(controlArea.removeFromRight(100).withSizeKeepingCentre(100, 40));
}

void PruvulazzuAudioProcessorEditor::timerCallback() {
    waveformVisualizer.setPlayheads(processor.getActiveGrainPositions());
    
    // Fetch the current Start and End knob values from the processor
    float start = processor.apvts.getRawParameterValue("start")->load();
    float end = processor.apvts.getRawParameterValue("end")->load();
    
    waveformVisualizer.setRange(start, end);
}

bool PruvulazzuAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray&) { return true; }

void PruvulazzuAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int) {
    juce::File file(files[0]);
    waveformVisualizer.setFile(file);
    auto newBuffer = loader.loadFile(file);
    processor.getSampleManager().updateCurrentBuffer(newBuffer);
}