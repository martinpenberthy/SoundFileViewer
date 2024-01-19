/*
  ==============================================================================

    fileComponent.h
    Created: 6 Dec 2023 5:25:47pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "fileComponentGUI.h"
#include "fileComponentAudio.h"
//==============================================================================


class fileComponent
{
public:
    fileComponent();
    ~fileComponent();
    
    fileComponentAudio* getFileComponentAudio();
    fileComponentGUI* getFileComponentGUI();
    
    void loadFile(juce::URL fileToLoad);
    void setWindowingFunction(juce::dsp::WindowingFunction<float>::WindowingMethod func);

private:
    fileComponentAudio* thisAudio;
    fileComponentGUI* thisGUI;

    juce::AudioThumbnailCache thumbnailCache{100};
    
    juce::AudioFormatManager formatManager;
};


