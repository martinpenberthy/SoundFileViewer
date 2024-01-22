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
    
    addAndMakeVisible(&buttonLoad);
    buttonLoad.setButtonText("Change File");
    buttonLoad.onClick = [this] {loadButtonClicked();};
    
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
    
    addAndMakeVisible(&waveformDisplay);
    addAndMakeVisible(&FFTDisplay);
    
    
    startTimer(30);
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

    //buttonLoad
    buttonLoad.setBounds(area.removeFromTop(containerNew1.proportionOfHeight(0.5f)).reduced(buttonMargin));
    //buttonPlay
    //buttonPlay.setBounds(area.removeFromLeft(containerNew1.proportionOfWidth(0.33f)).reduced(buttonMargin));
    buttonPlay.setBounds(area.removeFromTop(containerNew1.proportionOfHeight(0.5f)).reduced(buttonMargin));

    //labelFileName
    labelFileName.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelSampleRate
    labelSampleRate.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelBitDepth
    labelBitDepth.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    //labelPosition
    labelPosition.setBounds(areaLabels.removeFromTop(containerNew1Labels.proportionOfHeight(0.2f)));
    
    //waveformDisplay
    waveformDisplay.setBounds(area2);
    //FFTDisplay
    FFTDisplay.setBounds(area2.removeFromLeft(containerNew2.proportionOfWidth(0.5f)));
    
    /*buttonLoad.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
    buttonPlay.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(buttonMargin));
    //buttonLoad.setBounds(
    
    labelFileName.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.2f)).removeFromLeft(container.proportionOfWidth(0.2f)));
    labelSampleRate.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.2f)).removeFromLeft(container.proportionOfWidth(0.2f)));
    labelBitDepth.setBounds(bounds.removeFromTop(container.proportionOfHeight(0.2f)).removeFromLeft(container.proportionOfWidth(0.2f)));
    
    bounds.removeFromLeft(container.proportionOfWidth(0.2f));
    
    waveformDisplay.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.5f)));
    
    FFTDisplay.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.5f)));*/
}
    
void fileComponentGUI::loadButtonClicked()
{
    /*DBG("loadButtonClicked");

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
        tempStr.append(juce::String(player->sampleRate), 23);
        
        labelSampleRate.setText(tempStr, juce::NotificationType::dontSendNotification);
        
        auto tempStr2 = juce::String("Bit Depth: ");
        tempStr2.append(juce::String(player->bitDepth), 23);
        
        labelBitDepth.setText(tempStr2, juce::NotificationType::dontSendNotification);
        
    });*/
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
    
    
    auto tempStr = juce::String("Sample Rate: ");
    tempStr.append(juce::String(player->sampleRate), 23);
    labelSampleRate.setText(tempStr, juce::NotificationType::dontSendNotification);
    
    tempStr = juce::String("Bit Depth: ");
    tempStr.append(juce::String(player->bitDepth), 23);
    labelBitDepth.setText(tempStr, juce::NotificationType::dontSendNotification);
    
    tempStr = juce::String("Name: ");
    tempStr.append(juce::String(fileToLoad.getFileName()), 25);
    labelFileName.setText(tempStr, juce::NotificationType::dontSendNotification);
    

    
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
            //currentPositionLabel.setText ("Stopped", juce::dontSendNotification);
        }
}
