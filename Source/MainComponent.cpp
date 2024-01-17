#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }    
    
   
    
    /*addAndMakeVisible(&fileGUI);
    addAndMakeVisible(&fileGUI2);
    */
    addAndMakeVisible(openChooser);
    myChooser = std::make_unique<juce::FileChooser> ("Please select the .wav you want to load...",
                                                     juce::File{},
                                                     "*.wav");
    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems | juce::FileBrowserComponent::canSelectDirectories;
    

    myChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
    {
        auto result = chooser.getResult();
        auto results = chooser.getResults();
        
        //User picked a directory
        if(result.isDirectory())
        {
            
        }
        else if(results.size() == 1) //User picked one file
        {
            
        }else if (results.size() > 1 && results.size() <= 20) //User picked multiple files
        {
            
        }
            
        
        juce::URL audioURL = juce::URL{result};
        
    });
    
    
    
    addAndMakeVisible(file1.getFileComponentGUI());
    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    //fileComp1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    //fileComp2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    //fileComp3.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    /*for(int i = 0; i < 5; i++)
    {
        try{
            fileComponents.at(i)->prepareToPlay(samplesPerBlockExpected, sampleRate);
        }
        catch(std::exception e)
        {
            DBG(e.what());
            DBG("Prepare to play");
        }
    }*/
    DBG("prepareToPlay");
    

    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
    {
        auto curr = it->get();
        curr->sampRate = sampleRate;
        curr->sampsPerBlock = samplesPerBlockExpected;
        
        if(curr->fileLoaded)
            curr->prepareToPlay(samplesPerBlockExpected, sampleRate);
    }*/
    //fileAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    //fileAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    auto tempAudio = file1.getFileComponentAudio();
    
    tempAudio->prepareToPlay(samplesPerBlockExpected, sampleRate);
    //mixer.addInputSource(&fileAudio, false);
    //mixer.addInputSource(&fileAudio2, false);
    mixer.addInputSource(tempAudio, false);

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixer.getNextAudioBlock(bufferToFill);
    //fileAudio.getNextAudioBlock(bufferToFill);
    //fileAudio2.getNextAudioBlock(bufferToFill);


}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    auto tempAudio = file1.getFileComponentAudio();
    tempAudio->releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    auto heightFile1 = 100;
    auto marginFile1 = 7;

    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
        it->get()->setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));*/

    auto tempGUI = file1.getFileComponentGUI();
    /*fileGUI.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
    fileGUI2.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));*/
    tempGUI->setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
}
