/*
  ==============================================================================

    fileComponent.cpp
    Created: 6 Dec 2023 5:25:47pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include <JuceHeader.h>
#include "fileComponent.h"

//==============================================================================
fileComponent::fileComponent() : thumbnailCache (5), thumbnail (512, formatManager, thumbnailCache),
state(Stopped)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible (&openButton);
    openButton.setButtonText ("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible (&playButton);
    playButton.setButtonText ("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled (false);

    addAndMakeVisible (&stopButton);
    stopButton.setButtonText ("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled (false);
    
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
    
    fileLoaded = false;
}


fileComponent::~fileComponent()
{
}

void fileComponent::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                   juce::File{},
                                                   "*.wav");                     // [7]
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)     // [8]
    {
        //Get the file
        auto file = fc.getResult();
        //Check the file
        if (file != juce::File{})
        {
            //Make a reader for this file
            //reader = std::make_unique<juce::AudioFormatReader>(formatManager.createReaderFor (file));
            //reader = std::make_unique<juce::AudioFormatReader>();
            reader = formatManager.createReaderFor (file);
            
            
            if (reader != nullptr)
            {
                //Make the Format Reader Source from the reader
                auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
                //Set the transport source
               // transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled (true);
                //Set source for the thumbnail
                thumbnail.setSource(new juce::FileInputSource (file));
                //Get rid of the Reader Source
                readerSource.reset (newSource.release());
                //readerSource.reset ();
            }
        }
    });
    //if(readerSource->getLength() > 100)
    /*if(transportSource->getLengthInSeconds() > .01f)
        fileLoaded = true;
    else
        fileLoaded = false;*/
    //prepareToPlay(sampsPerBlock, sampRate);
}



void fileComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    //if (source == &transportSource)
        //transportSourceChanged();
    if (source == &thumbnail)
        thumbnailChanged();

}

void fileComponent::playButtonClicked()
{
    if ((state == Stopped) || (state == Paused))
        changeState (Starting);
    else if (state == Playing)
        changeState (Pausing);
}

void fileComponent::stopButtonClicked()
{
    if (state == Paused)
        changeState (Stopped);
    else
        changeState (Stopping);
    
}

void fileComponent::transportSourceChanged()
{
    
    if (transportSource->isPlaying())
        changeState (Playing);
    else if ((state == Stopping) || (state == Playing))
        changeState (Stopped);
    else if (Pausing == state)
        changeState (Paused);
}

void fileComponent::thumbnailChanged()
{
    repaint();
}


void fileComponent::changeState (TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case Stopped:
                playButton.setButtonText ("Play");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (false);
                transportSource->setPosition (0.0);
                break;

            case Starting:
                transportSource->start();
                break;

            case Playing:
                playButton.setButtonText ("Pause");
                stopButton.setButtonText ("Stop");
                stopButton.setEnabled (true);
                break;

            case Pausing:
                transportSource->stop();
                break;

            case Paused:
                playButton.setButtonText ("Resume");
                stopButton.setButtonText ("Return to Zero");
                break;

            case Stopping:
                transportSource->stop();
                break;
        }
    }
}

void fileComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource->prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void fileComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource->getNextAudioBlock (bufferToFill);
}


void fileComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource->releaseResources();
}

void fileComponent::paint (juce::Graphics& g)
{

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    
    auto lb = getLocalBounds();
    juce::Rectangle<int> thumbnailBounds (lb.getWidth() / 2, lb.getY(), lb.getWidth() / 2, lb.getHeight());

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded (g, thumbnailBounds);
    else
        paintIfFileLoaded (g, thumbnailBounds);

}

void fileComponent::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
    DBG("paintNoFile");
}

void fileComponent::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    DBG("paintFileLoaded");
    g.setColour (juce::Colours::white);
    g.fillRect (thumbnailBounds);

    g.setColour (juce::Colours::red);                               // [8]

    auto audioLength = (float) thumbnail.getTotalLength();                               // [12]
    thumbnail.drawChannels (g,                                      // [9]
                            thumbnailBounds,
                            0.0,                                    // start time
                            audioLength,                            // end time
                            1.0f);                                  // vertical zoom

    g.setColour (juce::Colours::green);

    auto audioPosition = (float) transportSource->getCurrentPosition();
    auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth()
                        + (float) thumbnailBounds.getX();                                // [13]
    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition,
                (float) thumbnailBounds.getBottom(), 2.0f);                              // [14]
}


void fileComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    /*float buttonWidth = 50;
    float buttonHeight = 25;
    
    openButton.setBounds(getLocalBounds().getX(), getLocalBounds().getY() - 100, buttonWidth, buttonHeight);
    playButton.setBounds(openButton.getX() + buttonWidth, getLocalBounds().getY() - 100, buttonWidth, buttonHeight);
    stopButton.setBounds(playButton.getX() + buttonWidth, getLocalBounds().getY() - 100, buttonWidth, buttonHeight);*/
    
    const auto container = getLocalBounds().reduced(4);
    auto bounds = container;
    
    auto buttonMargin = 4;
    
    //openButton.setBounds(area.removeFromTop(buttonHeight).removeFromLeft(buttonWidth).reduced(buttonMargin));
    //playButton.setBounds(area.removeFromTop(buttonHeight).removeFromLeft(buttonWidth).reduced(buttonMargin));
    
    openButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
    playButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
    stopButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
}

