#pragma once

#include <JuceHeader.h>
#include "fileComponent.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener, private juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void timerCallback() override;

private:
    //==============================================================================
    

    //fileComponent fileComp1;
    //fileComponent fileComp2;
    //fileComponent fileComp3;
    std::vector<std::unique_ptr<fileComponent>> fileComponents;
    
    std::vector<std::unique_ptr<fileComponent>>::iterator it;

    int numFiles = 2;
    //std::unique_ptr<juce::AudioFormatReaderSource> readerSource1;

    //juce::AudioFormatManager* formatManager;
    //std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
