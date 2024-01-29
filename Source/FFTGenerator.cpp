/*
  ==============================================================================

    FFTGenerator.cpp
    Created: 5 Jan 2024 2:58:20pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "FFTGenerator.h"

FFTGenerator::FFTGenerator(juce::AudioFormatManager* formatManagerToUse) : formatManager(formatManagerToUse), forwardFFT(fftOrder), os(2, 2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));
    //startTimerHz (30);
    
    window = new juce::dsp::WindowingFunction<float>(fftSize, juce::dsp::WindowingFunction<float>::triangular);
    
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
    
    avgLevel /= scopeSize;
    for(int i = 0; i < scopeSize; i++)
    {
        if(scopeDataSummed[i] < avgLevel)
            scopeDataSummed[i] = 0.0f;
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
    
    for (int i = 1; i < scopeSize; ++i)
    {
        //Map the scope levels to the size of the window
        /*g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap (scopeData[i - 1], 0.0f, 1.0f, (float) height-100, 0.0f),
                      (float) juce::jmap (i,     0, scopeSize - 1, 0, width),
                              juce::jmap (scopeData[i],     0.0f, 1.0f, (float) height-100, 0.0f) });*/
    
        g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width), //startX
                              juce::jmap (scopeDataSummed[i - 1], 0.0f, 1.0f, (float) height, 0.0f),//startY
                      (float) juce::jmap (i,     0, scopeSize - 1, 0, width), //endX
                              juce::jmap (scopeDataSummed[i],     0.0f, 1.0f, (float) height, 0.0f) }); //endY
        
        /*if(i % 150 == 0)
        {
            //Freq = (sr * index) / FFTSize
            std::cout << "Mod" << std::endl;
            
            auto freq = (sampleRate * i) / forwardFFT.getSize();
            std::cout << "freq: " << freq << std::endl;
            
            auto x = juce::jmap (i, 0, scopeSize - 1, 0, width);
            std::cout << "x: " << x << std::endl;
            
            g.drawVerticalLine(x, 0, 20);
            g.drawText(juce::String((int)freq), x, height - 20, 45, 20, juce::Justification::centred);
        }*/
    }
    
    for(int i = 0; i < forwardFFT.getSize(); i++)
    {
        auto freq = (sampleRate * i) / forwardFFT.getSize();

        /*if(i % 100 == 0)
        {
            std::cout << "i: " << i;
            std::cout << "freq: " << freq << std::endl;
        }
        else */
        if(i == 25) //|| i == 25)// || i == 50 || i == 200)
        {
            std::cout << "i: " << i;
            std::cout << "freq: " << freq << std::endl;
            
            auto x = juce::jmap (i, 0, scopeSize - 1, 0, width);
            std::cout << "x: " << x << std::endl;
            
            g.drawVerticalLine(x, height, 20);
            g.drawText(juce::String((int)freq), x, height - 20, 45, 20, juce::Justification::centred);
        }
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
    
    int buffSize = 256;
    //juce::dsp::AudioBlock<float> block(*fileBuffer.get(), fileBuffer->getNumChannels(), fileBuffer->getNumSamples());
    
    juce::dsp::AudioBlock<float> block (*fileBuffer.get());
    std::cout << "block created" << std::endl;
    /*
    for(int i = 0; i < block.getNumSamples(); i += buffSize)
    {
        for(int j = i; j < j + buffSize; j++)
        {
            
        }
        
    }
    */
    
    
    /*os.initProcessing(1);
    std::cout << "init processing" << std::endl;
    
    juce::dsp::AudioBlock<float> block (*fileBuffer.get());
    std::cout << "block created" << std::endl;
    
    os.processSamplesUp(block);
    std::cout << "process samples up" << std::endl;
    */
    
   /* //Create oversampling object
    juce::dsp::Oversampling<float>* overSamp; //= juce::dsp::Oversampling<float>(fileBuffer->getNumChannels(), 2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple);
    std::cout << "Oversampling object created" << std::endl;
    overSamp = new juce::dsp::Oversampling<float>(
                                                fileBuffer->getNumChannels(),
                                                2,
                                                juce::dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple,
                                                true,
                                                  true);;
    
    overSamp->reset();
    
    
    overSamp->initProcessing(128);
    std::cout << "initProcessing called" << std::endl;
    //Upsample
    //auto blockOverSamp = overSamp.processSamplesUp(juce::dsp::AudioBlock<float>(*fileBuffer.get()));
    auto tempBlock = juce::dsp::AudioBlock<float>(*fileBuffer);
    
    std::cout << "tempBlock created" << std::endl;
    std::cout << "Oversamp factor: " << overSamp->factorOversampling << std::endl;
    std::cout << "numChannels: " << overSamp->numChannels << std::endl;
    
    //overSamp->addOversamplingStage(juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, 0.1f, -50.0f, 0.1f, -45.0f);
    //std::cout << "Added oversampling stage" << std::endl;
    
    auto blockOverSamp = overSamp->processSamplesUp(tempBlock);
    std::cout << "blockOverSamp created and processed" << std::endl;
    
    juce::AudioBuffer<float> buffOverSamp = juce::AudioBuffer<float>(blockOverSamp.getNumChannels(), blockOverSamp.getNumSamples());
    
    
    for(int i = 0; i < buffOverSamp.getNumChannels(); i++)
    {
        std::cout << "Channel being copied over" << std::endl;
        for(int j = 0; j < buffOverSamp.getNumSamples(); j++)
        {
            buffOverSamp.setSample(i, j, blockOverSamp.getSample(i, j));
        }
    }
     
    
    truePeakL = buffOverSamp.getMagnitude(0, 0, buffOverSamp.getNumSamples());
    truePeakR = buffOverSamp.getMagnitude(1, 0, buffOverSamp.getNumSamples());
*/
    
}
