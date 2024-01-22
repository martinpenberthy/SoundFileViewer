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
#include "Waveform.h"
#include "FFTGenerator.h"

class fileComponentGUI : public juce::Component
{
public:
    fileComponentGUI(fileComponentAudio *player, juce::AudioFormatManager *formatManagerToUse, juce::AudioThumbnailCache *cacheToUse);
    ~fileComponentGUI();
    
    //Component overrides
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //void buttonClicked(juce::Button *button) override;
    
    void loadButtonClicked();
    void playButtonClicked();
    
    void loadURL(juce::URL fileToLoad);
    
    fileComponentAudio* player;
    juce::TextButton buttonPlay;
    juce::TextButton buttonLoad;

    Waveform waveformDisplay;
    FFTGenerator FFTDisplay;
private:
    std::unique_ptr<juce::FileChooser> myChooser;
    
    juce::Label labelFileName;
    juce::Label labelSampleRate;
    juce::Label labelBitDepth;
    juce::Label labelPosition;
    

};
