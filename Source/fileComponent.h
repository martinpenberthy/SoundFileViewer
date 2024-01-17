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
private:
    fileComponentAudio* thisAudio;
    fileComponentGUI* thisGUI;

    juce::AudioThumbnailCache thumbnailCache{100};
    
    juce::AudioFormatManager formatManager;
};

/*
class fileComponent  : public juce::AudioAppComponent, public juce::ChangeListener
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
        

    void changeState (TransportState newState);
    
    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void transportSourceChanged();
    void thumbnailChanged();
    
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    
    int sampsPerBlock;
    double sampRate;
    
    bool fileLoaded = false;
    
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    //std::unique_ptr<juce::AudioFormatReader> reader;
    juce::AudioFormatReader* reader;
    
    //juce::AudioTransportSource transportSource;
    juce::AudioTransportSource *transportSource;
    
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    
    
    TransportState state;
    

private:
    std::unique_ptr<juce::FileChooser> chooser;

    
    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (fileComponent)
};
*/
