/*
  ==============================================================================

    fileComponentGUI.h
    Created: 2 Jan 2024 2:51:10pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "fileComponentAudio.h"

class fileComponentGUI : public juce::Component, public juce::Button::Listener, public juce::Timer
{
public:
    fileComponentGUI(fileComponentAudio *player, juce::AudioFormatManager* formatManagerToUse, juce::AudioThumbnailCache cacheToUse);
    ~fileComponentGUI();
    
    //Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    fileComponentGUI* player;
    
    juce::TextButton buttonPlay;
private:
    
    juce::TextButton buttonLoad;
};
