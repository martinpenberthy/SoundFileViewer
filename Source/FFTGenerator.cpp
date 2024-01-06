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
    startTimerHz (30);

}

FFTGenerator::~FFTGenerator()
{
    
}

void FFTGenerator::loadURL(juce::URL audioURL)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));
    frameCount = 0;
    //Make a file out of the URL
    juce::File file = audioURL.getLocalFile();
    
    if(file.existsAsFile())
    {
        auto* reader = formatManager->createReaderFor (file);
        
        //Make an AudioBuffer
        fileBuffer = std::make_unique<juce::AudioBuffer<float>>();
        fileBuffer->setSize(reader->numChannels, (int)reader->lengthInSamples);
        
        //Put samples from reader into AudioBuffer
        reader->read(fileBuffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
        delete reader;

        
        if(fileBuffer.get() != nullptr)
            generateFFT();
        
    }
}

void FFTGenerator::generateFFT()
{
    if(fileBuffer.get() != nullptr)
    {
        for(int i = 0; i < fileBuffer->getNumChannels(); i++)
            for(int j = 0; j < fileBuffer->getNumSamples(); j++)
                pushNextSampleIntoFifo(fileBuffer->getSample(i, j));
    }
    
    for(int i = 0; i < scopeSize; i++)
        DBG(scopeDataSummed[i]);
    
    DBG("=================================================================");
    
    for(int i = 0; i < scopeSize; i++)
    {
        scopeDataSummed[i] = fmod(scopeDataSummed[i], 1);
        DBG(scopeDataSummed[i]);
    }
}


void FFTGenerator::pushNextSampleIntoFifo (float sample) noexcept
{
    // if the fifo contains enough data, set a flag to say
    // that the next frame should now be rendered..
    if (fifoIndex == fftSize)               // [11]
    {
        if (! nextFFTBlockReady)            // [12]
        {
            juce::zeromem (fftData, sizeof (fftData));
            memcpy (fftData, fifo, sizeof (fifo));
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;             // [12]
    
    if (nextFFTBlockReady)
    {
        frameCount++;
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}


void FFTGenerator::drawNextFrameOfSpectrum()
{
    // first apply a windowing function to our data
    window.multiplyWithWindowingTable (fftData, fftSize);       // [1]

    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform (fftData);  // [2]

    
    auto mindB = -100.0f;
    auto maxdB =    0.0f;

    //Do math to get the level value for each scopeData index
    for (int i = 0; i < scopeSize; ++i)                         // [3]
    {
        auto skewedProportionX = 1.0f - std::exp (std::log (1.0f - (float) i / (float) scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit (0, fftSize / 2, (int) (skewedProportionX * (float) fftSize * 0.5f));
        auto level = juce::jmap (juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (fftData[fftDataIndex])
                                                           - juce::Decibels::gainToDecibels ((float) fftSize)),
                                                            mindB, maxdB, 0.0f, 1.0f);

        //scopeData[i] = level;                                   // [4]
        scopeDataSummed[i] += (level / scopeSize);
        //scopeDataSummed[i] += level;
    }
}


void FFTGenerator::drawFrame (juce::Graphics& g)
{
    for (int i = 1; i < scopeSize; ++i)
    {
        auto width  = getLocalBounds().getWidth();
        auto height = getLocalBounds().getHeight();

        //Map the scope levels to the size of the window
        /*g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap (scopeData[i - 1], 0.0f, 1.0f, (float) height-100, 0.0f),
                      (float) juce::jmap (i,     0, scopeSize - 1, 0, width),
                              juce::jmap (scopeData[i],     0.0f, 1.0f, (float) height-100, 0.0f) });*/
    
        g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap (scopeDataSummed[i - 1], 0.0f, 1.0f, (float) height, 0.0f),
                      (float) juce::jmap (i,     0, scopeSize - 1, 0, width),
                              juce::jmap (scopeDataSummed[i],     0.0f, 1.0f, (float) height, 0.0f) });
    
    }
}

void FFTGenerator::timerCallback()
{
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}


void FFTGenerator::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setOpacity (1.0f);
    g.setColour (juce::Colours::white);
    drawFrame (g);
}
