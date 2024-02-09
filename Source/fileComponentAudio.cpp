/*
  ==============================================================================

    fileComponentAudio.cpp
    Created: 2 Jan 2024 2:51:00pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "fileComponentAudio.h"
fileComponentAudio::fileComponentAudio(juce::AudioFormatManager* formatManager) : formatManager(formatManager)
{
    DBG("fcAudio constructor");
}

fileComponentAudio::~fileComponentAudio()
{
    std::cout << "fileComponentAudio Destructor" << std::endl;
}

void fileComponentAudio::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void fileComponentAudio::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock (bufferToFill);
    
    if(!isPlaying())
        transportSource.setPosition(0.0f);
}
void fileComponentAudio::releaseResources()
{
    
}

/* loadURL from FileChooser and assign AudioFormatSourceReader* to transportSource and readerSource* */
void fileComponentAudio::loadURL(juce::URL audioURL)
{
    //Create InputStreamOptions object to pass to createReaderFoor
    auto inStreamOpts = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData);
    auto* reader = formatManager->createReaderFor(audioURL.createInputStream(inStreamOpts));
    
    
    if (reader != nullptr) // good file!
    {
        //Make an AudioFormatReaderSource
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        
        //reset to delete, release doesnt delete
        readerSource.reset (newSource.release());
        std::cout << "fCA: File loaded successfully." << std::endl;
        
        sampleRate = reader->sampleRate;
        bitDepth = reader->bitsPerSample;
        lengthInSecs = transportSource.getLengthInSeconds();
        
    }
    else
    {
      std::cout << "Something went wrong loading the file " << std::endl;
    }
    
}


void fileComponentAudio::start()
{
    std::cout <<"transport started" << std::endl;
    transportSource.start();
}
void fileComponentAudio::stop()
{
    std::cout <<"transport stopped" << std::endl;
    transportSource.stop();
    transportSource.setPosition(0.0f);
}

bool fileComponentAudio::isPlaying()
{
    return transportSource.isPlaying();
}

juce::AudioTransportSource* fileComponentAudio::getTransport()
{
    return &transportSource;
}

double fileComponentAudio::getCurrentPosition()
{
    return transportSource.getCurrentPosition();
}

