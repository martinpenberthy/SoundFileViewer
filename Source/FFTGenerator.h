/*
  ==============================================================================

    FFTGenerator.h
    Created: 5 Jan 2024 2:58:20pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FFTGenerator : public juce::Component, public juce::Timer
{
public:
    FFTGenerator(juce::AudioFormatManager*);
    ~FFTGenerator();
    
    void loadURL(juce::URL audioURL);
    void generateFFT();
    void pushNextSampleIntoFifo (float sample) noexcept;
    void drawNextFrameOfSpectrum();
    void drawFrame (juce::Graphics& g);
    
    void timerCallback();
    
    void paint (juce::Graphics& g) override;

    
    enum
    {
        fftOrder  = 10,
        fftSize   = 1 << fftOrder,
        scopeSize = 256
    };
private:
    juce::AudioFormatManager* formatManager;
    
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    
    float fifo [fftSize];                           // [6]
    float fftData [2 * fftSize];                    // [7]
    int fifoIndex = 0;                              // [8]
    bool nextFFTBlockReady = false;                 // [9]
    float scopeDataSummed [scopeSize];
    
    int frameCount = 0;
    
    std::unique_ptr<juce::AudioBuffer<float>> fileBuffer;
    
    //std::unique_ptr<juce::AudioFormatReader> reader;

};
