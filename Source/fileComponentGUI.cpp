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
                                   : player(player), waveformDisplay(formatManagerToUse, cacheToUse, player), FFTDisplay(formatManagerToUse)
{
    DBG("fcGUI constructor");
    
    addAndMakeVisible(&buttonToggleDelete);
    
    addAndMakeVisible(&buttonPlay);
    buttonPlay.setButtonText("Play/Stop");
    buttonPlay.onClick = [this] {playButtonClicked();};
    
    addAndMakeVisible(&labelFileName);
    labelFileName.setText("Name: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&labelSampleRate);
    labelSampleRate.setText("Sample Rate: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&labelBitDepth);
    labelBitDepth.setText("Bit Depth: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&labelPosition);
    labelPosition.setText("Position: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&labelLength);
    labelLength.setText("Length: ", juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(&waveformDisplay);
    addAndMakeVisible(&FFTDisplay);
    
    
    startTimer(30);
}

fileComponentGUI::~fileComponentGUI()
{
    std::cout << "fileComponentGUI Destructor" << std::endl;
}


void fileComponentGUI::paint(juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).withSaturationHSL(0.1).darker());
}

void fileComponentGUI::resized()
{
    const auto container = getLocalBounds().reduced(4);
    auto bounds = container;
    
    auto buttonMargin = 5;
    
    
    auto area = getLocalBounds();
    auto area2 = getLocalBounds();
    auto areaLabels = getLocalBounds();
    
    area = area.removeFromLeft(proportionOfWidth(0.25f));
    areaLabels = area.removeFromRight(area.proportionOfWidth(0.6f));
    area2 = area2.removeFromRight(proportionOfWidth(0.75f));
    
    auto const containerNew1 = area;
    auto const containerNew1Labels = areaLabels;
    auto const containerNew2 = area2;

    //buttonPlay
    buttonPlay.setBounds(area.removeFromTop(containerNew1.proportionOfHeight(0.5f)).reduced(buttonMargin));
    //buttonToggleDelete
    buttonToggleDelete.setBounds(area.removeFromTop(containerNew1.proportionOfHeight(0.5f)).reduced(buttonMargin));


    //labelFileName
    labelFileName.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelSampleRate
    labelSampleRate.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelBitDepth
    labelBitDepth.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelPosition
    labelPosition.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelLength
    labelLength.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    
    //waveformDisplay
    waveformDisplay.setBounds(area2);
    //FFTDisplay
    FFTDisplay.setBounds(area2.removeFromLeft(containerNew2.proportionOfWidth(0.5f)));
}


void fileComponentGUI::playButtonClicked()
{
    DBG("Play button clicked");
    std::cout <<"Play button clicked" << std::endl;
    
    if(player->isPlaying())
        player->stop();
    else
        player->start();
}


void fileComponentGUI::loadURL(juce::URL fileToLoad)
{
    player->loadURL(fileToLoad);
    waveformDisplay.loadURL(fileToLoad);
    FFTDisplay.loadURL(fileToLoad);
    FFTDisplay.sampleRate = player->sampleRate;
    
    auto tempStr = juce::String("Sample Rate: ");
    tempStr.append(juce::String(player->sampleRate), 23);
    labelSampleRate.setText(tempStr, juce::NotificationType::dontSendNotification);
    
    tempStr = juce::String("Bit Depth: ");
    tempStr.append(juce::String(player->bitDepth), 23);
    labelBitDepth.setText(tempStr, juce::NotificationType::dontSendNotification);
    
    tempStr = juce::String("Name: ");
    tempStr.append(juce::String(fileToLoad.getFileName()), 25);
    labelFileName.setText(tempStr, juce::NotificationType::dontSendNotification);
    
    tempStr = juce::String("Length: ");
    
    juce::RelativeTime position (player->lengthInSecs);
    auto minutes = ((int) position.inMinutes()) % 60;
    auto seconds = ((int) position.inSeconds()) % 60;
    auto millis  = ((int) position.inMilliseconds()) % 1000;
    juce::String positionFormatted = juce::String::formatted ("%02d:%02d:%03d", minutes, seconds, millis);
    
    tempStr.append(positionFormatted, 25);
    labelLength.setText(tempStr, juce::NotificationType::dontSendNotification);

    

    //player->
}


void fileComponentGUI::timerCallback()
{
    if (player->isPlaying())
    {
        juce::RelativeTime position (player->getCurrentPosition());
        
        auto minutes = ((int) position.inMinutes()) % 60;
        auto seconds = ((int) position.inSeconds()) % 60;
        auto millis  = ((int) position.inMilliseconds()) % 1000;

           // auto positionString = juce::String::formatted ("%02d:%02d:%03d", minutes, seconds, millis);
        juce::String positionFormatted = juce::String::formatted ("%02d:%02d:%03d", minutes, seconds, millis);

        auto tempStr = juce::String("Position: ");
        tempStr.append(positionFormatted, 25);
        labelPosition.setText(tempStr, juce::NotificationType::dontSendNotification);
            //currentPositionLabel.setText (positionString, juce::dontSendNotification);
    }
    else
    {
        //juce::String positionFormatted = juce::String::formatted ("%02d:%02d:%03d", 0.0f, 0.0f, 0.0f);

        auto tempStr = juce::String("Position: 00:00:000");
        labelPosition.setText(tempStr, juce::NotificationType::dontSendNotification);

        /*tempStr.append(positionFormatted, 25);
        labelPosition.setText(tempStr, juce::NotificationType::dontSendNotification);*/

    }
}
