#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

class WaveformComponent : public juce::Component, public juce::ChangeListener {
public:
    WaveformComponent(juce::AudioFormatManager& formatManager)
        : thumbnail(512, formatManager, thumbnailCache) 
    {
        thumbnail.addChangeListener(this);
    }

    void setFile(const juce::File& file) {
        thumbnail.setSource(new juce::FileInputSource(file));
    }

    void setPlayheads(const std::vector<float>& positions) {
        currentPositions = positions;
        repaint();
    }

    void setRange(float start, float end) {
        if (start != startRange || end != endRange) {
            startRange = start;
            endRange = end;
            repaint();
        }
    }

    // void paint(juce::Graphics& g) override {
    //     g.fillAll(juce::Colours::black);
    //     g.setColour(juce::Colours::cyan);

    //     if (thumbnail.getNumChannels() > 0) {
    //         thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
    //     } else {
    //         g.drawText("Drop an audio file here", getLocalBounds(), juce::Justification::centred);
    //     }
    // }
    void paint(juce::Graphics& g) override {
        // Draw Waveform (Existing code)
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::cyan.withAlpha(0.8f)); // Slightly transparent for layering
        if (thumbnail.getNumChannels() > 0){
            thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
        } else {
            g.drawText("Drop an audio file here", getLocalBounds(), juce::Justification::centred);
        }

        // Draw active region range (Start/End)
        g.setColour(juce::Colours::white.withAlpha(0.3f)); // Semi-transparent yellow
        auto startX = startRange * getWidth();
        auto endX = endRange * getWidth();
        g.fillRect(startX, 0.0f, endX - startX, (float)getHeight());

        // Draw the boundaries of the active region (green for start, red for end)
        g.setColour(juce::Colours::green);
        g.drawLine(startX, 0, startX, getHeight(), 2.0f);
        g.setColour(juce::Colours::red);
        g.drawLine(endX, 0, endX, getHeight(), 2.0f);

        // Draw the "Dust" (Multiple Playheads), to be tested
        // g.setColour(juce::Colours::white.withAlpha(0.6f)); // Semi-transparent
        g.setColour(juce::Colours::white); // Opaque for testing
        for (float pos : currentPositions) {
            auto x = pos * getWidth();
            g.drawLine(x, 0, x, getHeight(), 1.5f);  //thicker line for visibility, TODO should be adjusted later as needed
        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster*) override { repaint(); }

private:
    std::vector<float> currentPositions;
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail;
    float startRange = 0.0f;
    float endRange = 1.0f;
};