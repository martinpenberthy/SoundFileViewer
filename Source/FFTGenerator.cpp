/*
  ==============================================================================

    FFTGenerator.cpp
    Created: 5 Jan 2024 2:58:20pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "FFTGenerator.h"

FFTGenerator::FFTGenerator() : forwardFFT(fftOrder), window (fftSize, juce::dsp::WindowingFunction<float>::triangular)
{
    juce::zeromem(scopeDataSummed, sizeof(scopeDataSummed));

}

FFTGenerator::~FFTGenerator()
{
    
}

void FFTGenerator::loadURL(juce::URL audioURL)
{
    
}

void FFTGenerator::generateFFT()
{
    
}


