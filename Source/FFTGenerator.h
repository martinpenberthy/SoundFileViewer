/*
  ==============================================================================

    FFTGenerator.h
    Created: 5 Jan 2024 2:58:20pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FFTGenerator : public juce::Component
{
public:
    FFTGenerator(juce::AudioFormatManager*);
    ~FFTGenerator();
    
    void loadURL(juce::URL audioURL);
    void generateFFT();
    void pushNextSampleIntoFifo (float sample) noexcept;
    void drawNextFrameOfSpectrum();
    void drawFrame (juce::Graphics& g);
    
        
    void paint (juce::Graphics& g) override;
    void setWindowingFunction(juce::dsp::WindowingFunction<float>::WindowingMethod func);

    void getLoudnessMeasurements();
    void drawLoudnessValues(juce::Graphics& g);
    enum
    {
        fftOrder  = 10,
        fftSize   = 1 << fftOrder,
        scopeSize = 1024
    };
    
    double sampleRate = 0.0f;
private:
    juce::Graphics* graphicsPtr;
    juce::AudioFormatManager* formatManager;
    
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float>* window;
    
    float fifo [fftSize];                           // [6]
    float fftData [2 * fftSize];                    // [7]
    int fifoIndex = 0;                              // [8]
    bool nextFFTBlockReady = false;                 // [9]
    float scopeDataSummed [scopeSize];
    
    float avgLevel = 0.0f;
    

    float RMSLevelL = 0.0f;
    float RMSLevelR = 0.0f;
    
    float truePeakL = 0.0f;
    float truePeakR = 0.0f;
    
    float peakL = 0.0f;
    float peakR = 0.0f;
    
    std::unique_ptr<juce::AudioBuffer<float>> fileBuffer;
    
    juce::Slider sliderFreqHighlight;
    
    //std::unique_ptr<juce::AudioFormatReader> reader;

};
