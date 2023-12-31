#pragma once

#include <JuceHeader.h>
//#include "fileComponent.h"
#include "fileComponentAudio.h"
#include "fileComponentGUI.h"
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


private:
    //==============================================================================
    void timerCallback() override;

    //fileComponent fileComp1;
    //fileComponent fileComp2;
    //fileComponent fileComp3;
    /*std::vector<std::unique_ptr<fileComponent>> fileComponents;
    
    std::vector<std::unique_ptr<fileComponent>>::iterator it;

    int numFiles = 1;
    std::shared_ptr<juce::AudioTransportSource> globalTransportSource;
    */
    //std::unique_ptr<juce::AudioFormatReaderSource> readerSource1;
    
    //juce::AudioFormatManager* formatManager;
    //std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioThumbnailCache thumbnailCache{100};
    
    juce::AudioFormatManager formatManager;
    fileComponentAudio fileAudio{&formatManager};
    fileComponentGUI fileGUI{&fileAudio, &formatManager, &thumbnailCache};
    
    fileComponentAudio fileAudio2{&formatManager};
    fileComponentGUI fileGUI2{&fileAudio2, &formatManager, &thumbnailCache};
    
    juce::MixerAudioSource mixer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
