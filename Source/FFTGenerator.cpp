/*
  ==============================================================================

    FFTGenerator.cpp
    Created: 5 Jan 2024 2:58:20pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "FFTGenerator.h"

FFTGenerator::FFTGenerator(juce::AudioFormatManager* formatManagerToUse) : formatManager(formatManagerToUse), forwardFFT(fftOrder)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));
    //startTimerHz (30);
    
    window = new juce::dsp::WindowingFunction<float>(fftSize, juce::dsp::WindowingFunction<float>::triangular);
    addAndMakeVisible(&sliderFreqHighlight);
    sliderFreqHighlight.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    sliderFreqHighlight.setRange(0.0f, 1.0f);
}

FFTGenerator::~FFTGenerator()
{
    delete window;
}

void FFTGenerator::loadURL(juce::URL audioURL)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));
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
        {
            getLoudnessMeasurements();
            generateFFT();
        }
    }
}

void FFTGenerator::generateFFT()
{
    std::cout << "generateFFT" << std::endl;
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));

    if(fileBuffer.get() != nullptr)
    {
        for(int i = 0; i < fileBuffer->getNumChannels(); i++)
            for(int j = 0; j < fileBuffer->getNumSamples(); j++)
                pushNextSampleIntoFifo(fileBuffer->getSample(i, j));
    }
    
    /*for(int i = 0; i < scopeSize; i++)
        DBG(scopeDataSummed[i]);*/
    
    //DBG("=================================================================");
    
    for(int i = 0; i < scopeSize; i++)
    {
        //scopeDataSummed[i] = fmod(scopeDataSummed[i], 1) *  2.0f;
        scopeDataSummed[i] = fmod(scopeDataSummed[i], 1);
        std::cout << scopeDataSummed[i] << std::endl;
        avgLevel += scopeDataSummed[i];
    }
    
    /*avgLevel /= scopeSize;
    for(int i = 0; i < scopeSize; i++)
    {
        if(scopeDataSummed[i] < avgLevel)
            scopeDataSummed[i] = 0.0f;
    }*/
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
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}


void FFTGenerator::drawNextFrameOfSpectrum()
{
    // first apply a windowing function to our data
    window->multiplyWithWindowingTable (fftData, fftSize);       // [1]

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
    auto width  = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    
    auto heightScope = height - 20;
    auto bottomScope = 15.0f;
    
    for (int i = 1; i < scopeSize; ++i)
    {
        //Map the scope levels to the size of the window
        /*g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap (scopeData[i - 1], 0.0f, 1.0f, (float) height-100, 0.0f),
                      (float) juce::jmap (i,     0, scopeSize - 1, 0, width),
                              juce::jmap (scopeData[i],     0.0f, 1.0f, (float) height-100, 0.0f) });*/
    
        g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width), //startX
                              juce::jmap (scopeDataSummed[i - 1], 0.0f, 1.0f, (float) heightScope, bottomScope),//startY
                      (float) juce::jmap (i,     0, scopeSize - 1, 0, width), //endX
                              juce::jmap (scopeDataSummed[i],     0.0f, 1.0f, (float) heightScope, bottomScope) }); //endY
        
    }
    drawLoudnessValues(g);
}

void FFTGenerator::drawLoudnessValues(juce::Graphics& g)
{
    auto xOffset = 0;

    //RMSlevelL
    auto tempDouble = juce::Decibels::gainToDecibels(RMSLevelL);
    auto tempString = juce::String(tempDouble);
    tempString = tempString.substring(0, tempString.indexOf(".") + 2);
    
    g.drawText(juce::String("RMS L: "), xOffset, 0, 50, 20, juce::Justification::right);
    g.drawText(tempString, xOffset += 30, 0, 50, 20, juce::Justification::right);
    
    //RMSlevelR
    tempDouble = juce::Decibels::gainToDecibels(RMSLevelR);
    tempString = juce::String(tempDouble);
    tempString = tempString.substring(0, tempString.indexOf(".") + 2);
    
    g.drawText(juce::String("R: "), xOffset += 30, 0, 50, 20, juce::Justification::right);
    g.drawText(tempString, xOffset += 30, 0, 50, 20, juce::Justification::right);
    
    //PeakL
    tempDouble = juce::Decibels::gainToDecibels(peakL);
    tempString = juce::String(tempDouble);
    tempString = tempString.substring(0, tempString.indexOf(".") + 2);
    
    g.drawText(juce::String("P L: "), xOffset += 30, 0, 50, 20, juce::Justification::right);
    g.drawText(tempString, xOffset += 30, 0, 50, 20, juce::Justification::right);
    
    //PeakR
    tempDouble = juce::Decibels::gainToDecibels(peakR);
    tempString = juce::String(tempDouble);
    tempString = tempString.substring(0, tempString.indexOf(".") + 2);
    
    g.drawText(juce::String("R: "), xOffset += 30, 0, 50, 20, juce::Justification::right);
    g.drawText(tempString, xOffset += 30, 0, 50, 20, juce::Justification::right);
    
    
    auto width  = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    auto heightScope = height - 20;

    //100 hz
    int x100hz = width / 28;
    g.drawVerticalLine(x100hz, height - 5, height);
    g.drawText(juce::String("100hz"), x100hz + 3, height - 15, 50, 20, juce::Justification::left);
    
    //1000 hz
    int x1000hz = width / 5;
    g.drawVerticalLine(x1000hz, height - 5, height);
    g.drawText(juce::String("1khz"), x1000hz + 3, height - 15, 50, 20, juce::Justification::left);

    
    //5000 hz
    int x5000hz = width / 1.5;
    g.drawVerticalLine(x5000hz, height - 5, height);
    g.drawText(juce::String("5khz"), x5000hz + 3, height - 15, 50, 20, juce::Justification::left);
    
}



void FFTGenerator::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setOpacity (1.0f);
    g.setColour (juce::Colours::white);
    drawFrame (g);
    
    //auto skewedProportionX = std::exp (std::log (1.0f - (float) 1 / (float) getLocalBounds().getWidth()));
    
    auto width = getLocalBounds().getWidth();
    
   // auto increments = (float)width / 5000.0f;
    //std::cout << increments << std::endl;
    //auto skewedProportionX = std::log();
    //g.drawVerticalLine(std::log(increments * 5000.0f), 1.0f, 10.0f);
    
    //std::cout << skewedProportionX << std::endl;
    
    //g.drawText("12345", getWidth()-40, getHeight(), 40, 40, juce::NotificationType::dontSendNotification);
    //g.drawText("100", getLocalBounds(), juce::Justification::topLeft, true);
    //g.drawText("1000", getLocalBounds(), juce::Justification::centredTop, true);
    
}


void FFTGenerator::setWindowingFunction(juce::dsp::WindowingFunction<float>::WindowingMethod func)
{
    delete window;
    window = new juce::dsp::WindowingFunction<float>(fftSize, func);
    generateFFT();
}

void FFTGenerator::getLoudnessMeasurements()
{

    for(int i = 0; i < fileBuffer->getNumChannels(); i++)
    {
        if(i == 0)
            RMSLevelL = fileBuffer->getRMSLevel(i, 0, fileBuffer->getNumSamples());
        else
            RMSLevelR = fileBuffer->getRMSLevel(i, 0, fileBuffer->getNumSamples());
    }
    
    for(int i = 0; i < fileBuffer->getNumChannels(); i++)
    {
        if(i == 0)
            peakL = fileBuffer->getMagnitude(i, 0, fileBuffer->getNumSamples());
        else
            peakR = fileBuffer->getMagnitude(i, 0, fileBuffer->getNumSamples());
    }
  
}
