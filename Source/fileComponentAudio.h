/*
  ==============================================================================

    fileComponentAudio.h
    Created: 2 Jan 2024 2:51:00pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class fileComponentAudio : public juce::AudioSource
{
public:
    fileComponentAudio(juce::AudioFormatManager* formatManager);
    ~fileComponentAudio() override;
    
    
    /* AudioSource override */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void loadURL(juce::URL audioURL);

    void start();
    void stop();
    
    bool isPlaying();
    juce::AudioTransportSource* getTransport();
    double getCurrentPosition();
    
    double sampleRate;
    unsigned int bitDepth;
    double lengthInSecs;
private:
    juce::AudioFormatManager* formatManager;
    
    //    this is a pointer as we dont want to contruct it until we have a file to play
    //    it is a unique ptr as that should be what pointers always start with, until they need to be passed around, then change
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;   //this is a type of audioSource
        //the constructor for ResamplingAudioSource receives a pointer to an AudioSource, which is transportSource in this case
    juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};
};
