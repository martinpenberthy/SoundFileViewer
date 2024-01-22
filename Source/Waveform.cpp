/*
  ==============================================================================

    Waveform.cpp
    Created: 4 Jan 2024 4:17:08pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "Waveform.h"

Waveform::Waveform(juce::AudioFormatManager* formatManagerToUse, juce::AudioThumbnailCache* cacheToUse, fileComponentAudio* player) : thumbnail(1000, *formatManagerToUse, *cacheToUse)
{
    fCA = player;
    thumbnail.addChangeListener(this);
    fileLoaded = false;
    
    startTimer(30);
}

Waveform::~Waveform()
{
    
}

void Waveform::paint(juce::Graphics& g)
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

void Waveform::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
    DBG("paintNoFile");
}

void Waveform::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
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

    auto audioPosition = (float) fCA->getTransport()->getCurrentPosition();
    auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth()
                        + (float) thumbnailBounds.getX();
    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition,
                (float) thumbnailBounds.getBottom(), 2.0f);
}

void Waveform::resized()
{
    
}

/* this method listens for any changes, if any, repaints the updated pos in wfd */
void Waveform::changeListenerCallback (juce::ChangeBroadcaster *source)
{
    repaint();
}

/* if valid file has been loaded, sets class bool fileLoaded to true (repaint waveform)*/
void Waveform::loadURL(juce::URL audioURL)
{
    thumbnail.clear();
    //unpack audioURL, turn it into an InputSource, then call setSource
    fileLoaded  = thumbnail.setSource(new juce::URLInputSource(audioURL));
    if (fileLoaded)
    {
        repaint();
    }
    else {
        std::cout << "wfd: not loaded! " << std::endl;
    }

}

void Waveform::timerCallback()
{
    repaint();
    
    
    if (fCA->isPlaying())
        {
            juce::RelativeTime position (fCA->getCurrentPosition());

            auto minutes = ((int) position.inMinutes()) % 60;
            auto seconds = ((int) position.inSeconds()) % 60;
            auto millis  = ((int) position.inMilliseconds()) % 1000;

           // auto positionString = juce::String::formatted ("%02d:%02d:%03d", minutes, seconds, millis);
            positionFormatted = juce::String::formatted ("%02d:%02d:%03d", minutes, seconds, millis);

            //currentPositionLabel.setText (positionString, juce::dontSendNotification);
        }
        else
        {
            //currentPositionLabel.setText ("Stopped", juce::dontSendNotification);
        }
}
