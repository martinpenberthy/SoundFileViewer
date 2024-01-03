/*
  ==============================================================================

    fileComponentGUI.cpp
    Created: 2 Jan 2024 2:51:10pm
    Author:  Martin Penberthy

  ==============================================================================
*/

#include "fileComponentGUI.h"


fileComponentGUI::fileComponentGUI(fileComponentAudio *player) : player(player)
{
    addAndMakeVisible(&buttonLoad);
    addAndMakeVisible(&buttonPlay);
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
}



void fileComponentGUI::buttonClicked(juce::Button *button)
{
    if(button == &buttonPlay)
    {
        if(button->getToggleState())
            player->start();
        else
            player->stop();
    }
    else if(button == &buttonLoad)
    {
        std::unique_ptr<juce::FileChooser> myChooser;
        myChooser = std::make_unique<juce::FileChooser> ("Please select the moose you want to load...",
                                                   juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                                                   "*.moose");
        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories;
     
        myChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
        {
            juce::URL audioURL = juce::URL{chooser.getResult()};
            player->loadURL(audioURL);
        });
    }
}
