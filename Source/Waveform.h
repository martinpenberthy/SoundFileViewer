/*
  ==============================================================================

    Waveform.h
    Created: 4 Jan 2024 4:17:08pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "fileComponentAudio.h"

class Waveform : public juce::Component, public juce::ChangeListener, public juce::Timer
{
public:
    Waveform(juce::AudioFormatManager* formatManagerToUse, juce::AudioThumbnailCache* cacheToUse, fileComponentAudio* player);
    ~Waveform();
    
    void paint (juce::Graphics&) override;
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    
    void resized() override;
    
    void timerCallback() override;
    
    /* this method listens for any changes, if any, repaints the updated pos in wfd */
    void changeListenerCallback (juce::ChangeBroadcaster *source) override;
    /* if valid file has been loaded, sets class bool fileLoaded to true (repaint waveform)*/
    void loadURL(juce::URL audioURL);

    //juce::String positionFormatted = juce::String::formatted ("%02d:%02d:%03d", 0.0f, 0.0f, 0.0f);
private:
    juce::AudioThumbnail thumbnail;
    bool fileLoaded;
    fileComponentAudio* fCA;
};
