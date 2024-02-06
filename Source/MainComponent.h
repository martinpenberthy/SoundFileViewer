#pragma once

#include <JuceHeader.h>
#include "fileComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
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
    
    fileComponent* makeNewFileComponent(juce::URL audioURL);

private:
    //==============================================================================
    float myBlockSize = 0.0f;
    float mySampleRate = 0.0f;
    
    fileComponent* file1 = nullptr;
    
    juce::MixerAudioSource mixer;
    
    std::unique_ptr<juce::FileChooser> myChooser;
    juce::TextButton buttonOpenChooser;
    
    juce::ComboBox comboWindowType;
    
    std::vector<fileComponent*> filesVec;
    
    juce::Viewport myViewport;
    juce::Component contComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
