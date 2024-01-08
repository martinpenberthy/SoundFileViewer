/*
  ==============================================================================

    fileComponentGUI.cpp
    Created: 2 Jan 2024 2:51:10pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "fileComponentGUI.h"


fileComponentGUI::fileComponentGUI(fileComponentAudio *player,
                                   juce::AudioFormatManager* formatManagerToUse,
                                   juce::AudioThumbnailCache* cacheToUse)
                                   : player(player), waveformDisplay(formatManagerToUse, cacheToUse), FFTDisplay(formatManagerToUse)
{
    DBG("fcGUI constructor");
    addAndMakeVisible(&buttonLoad);
    buttonLoad.setButtonText("Open...");
    buttonLoad.onClick = [this] {loadButtonClicked();};
    
    addAndMakeVisible(&buttonPlay);
    buttonPlay.setButtonText("Play/Stop");
    buttonPlay.onClick = [this] {playButtonClicked();};
    
    addAndMakeVisible(&labelSampleRate);
    labelSampleRate.setText("Sample Rate: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&labelBitDepth);
    labelBitDepth.setText("Bit Depth: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&waveformDisplay);
    addAndMakeVisible(&FFTDisplay);
}

fileComponentGUI::~fileComponentGUI()
{
    
}


void fileComponentGUI::paint(juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).withSaturationHSL(0.1).darker());
}

void fileComponentGUI::resized()
{
    const auto container = getLocalBounds().reduced(4);
    auto bounds = container;
    
    auto buttonMargin = 4;
    
    buttonLoad.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
    buttonPlay.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
    
    labelSampleRate.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.2f)).removeFromLeft(container.proportionOfWidth(0.1f)));
    labelBitDepth.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.2f)).removeFromLeft(container.proportionOfWidth(0.1f)));
    
    waveformDisplay.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)));
    
    FFTDisplay.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)));
}
    
void fileComponentGUI::loadButtonClicked()
{
    DBG("loadButtonClicked");

    myChooser = std::make_unique<juce::FileChooser> ("Please select the .wav you want to load...",
                                                     juce::File{},
                                                     "*.wav");
    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
    

    myChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
    {
        DBG("Chooser callback");
        juce::URL audioURL = juce::URL{chooser.getResult()};
        player->loadURL(audioURL);
        waveformDisplay.loadURL(audioURL);
        FFTDisplay.loadURL(audioURL);
        
        auto tempStr = juce::String("Sample Rate: ");
        tempStr.append(juce::String(player->sampleRate), 7);
        
        labelSampleRate.setText(tempStr, juce::NotificationType::dontSendNotification);
    });
}

void fileComponentGUI::playButtonClicked()
{
    DBG("Play button clicked");
    
    if(player->isPlaying())
        player->stop();
    else
        player->start();
}

