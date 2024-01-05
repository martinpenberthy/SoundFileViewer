/*
  ==============================================================================

    Waveform.h
    Created: 4 Jan 2024 4:17:08pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Waveform : public juce::Component
{
public:
    Waveform(juce::AudioFormatManager* formatManagerToUse, juce::AudioThumbnailCache* cacheToUse);
    ~Waveform();
    
private:
    juce::AudioThumbnail thumbnail;
};
