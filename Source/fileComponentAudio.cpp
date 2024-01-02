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
    
}

fileComponentAudio::~fileComponentAudio()
{
    
}

void fileComponentAudio::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    
}
void fileComponentAudio::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    
}
void fileComponentAudio::releaseResources()
{
    
}

/* loadURL from FileChooser and assign AudioFormatSourceReader* to transportSource and readerSource* */
void fileComponentAudio::loadURL(juce::URL audioURL)
{
    auto* reader = formatManager->createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        //reset to delete, release doesnt delete
        readerSource.reset (newSource.release());
        std::cout << "File loaded successfully." << std::endl;
    }
    else
    {
      std::cout << "Something went wrong loading the file " << std::endl;
    }
    
}


void fileComponentAudio::start()
{
    transportSource.start();
}
void fileComponentAudio::stop()
{
    transportSource.stop(); //pausing only
}
