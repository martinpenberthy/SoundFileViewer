/*
  ==============================================================================

    FFTGenerator.cpp
    Created: 5 Jan 2024 2:58:20pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "FFTGenerator.h"

FFTGenerator::FFTGenerator(juce::AudioFormatManager* formatManagerToUse) : formatManager(formatManagerToUse), forwardFFT(fftOrder), window (fftSize, juce::dsp::WindowingFunction<float>::triangular)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));

}

FFTGenerator::~FFTGenerator()
{
    
}

void FFTGenerator::loadURL(juce::URL audioURL)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));
    
    //Make a file out of the URL
    juce::File file = audioURL.getLocalFile();
    
    
    auto* reader = formatManager->createReaderFor (file);
    
    //Make an AudioBuffer
    fileBuffer = std::make_unique<juce::AudioBuffer<float>>();
    fileBuffer->setSize(reader->numChannels, (int)reader->lengthInSamples);
    
    //Put samples from reader into AudioBuffer
    reader->read(fileBuffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
    
    if(fileBuffer.get() != nullptr)
        generateFFT();

    delete reader;
    /*for(int i = 0; i < fileBuffer->getNumChannels(); i++)
        for(int j = 0; j < fileBuffer->getNumSamples(); j++)
            DBG(fileBuffer->getSample(i, j));*/
    /*if (fileBuffer-)
    {
        repaint();
    }
    else {
        std::cout << "wfd: not loaded! " << std::endl;
    }*/
    

}

void FFTGenerator::generateFFT()
{
    
}


