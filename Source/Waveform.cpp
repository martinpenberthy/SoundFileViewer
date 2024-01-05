/*
  ==============================================================================

    Waveform.cpp
    Created: 4 Jan 2024 4:17:08pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "Waveform.h"

Waveform::Waveform(juce::AudioFormatManager* formatManagerToUse, juce::AudioThumbnailCache* cacheToUse) : thumbnail(1000, *formatManagerToUse, *cacheToUse)
{
    
}

Waveform::~Waveform()
{
    
}
