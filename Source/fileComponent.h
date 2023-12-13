/*
  ==============================================================================

    fileComponent.h
    Created: 6 Dec 2023 5:25:47pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class fileComponent  : public juce::AudioAppComponent, public juce::ChangeListener, private juce::Timer
{
public:
    fileComponent();

    ~fileComponent() override;

    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    
    void paint (juce::Graphics&) override;
    void resized() override;
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
        
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void changeState (TransportState newState);
    
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    
    void transportSourceChanged();
    void thumbnailChanged();
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void timerCallback() override;

    
    int sampsPerBlock;
    double sampRate;
private:
    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    //juce::AudioFormatManager *formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    //std::shared_ptr<juce::AudioFormatReaderSource> readerSource;
    //juce::AudioFormatReaderSource* readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (fileComponent)
};
