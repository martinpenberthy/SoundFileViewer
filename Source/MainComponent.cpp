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
    
    //formatManager = new juce::AudioFormatManager();
    //fileComp1 = new fileComponent(formatManager);
    
    //addAndMakeVisible(&fileComp1);
    //addAndMakeVisible(&fileComp2);
    //addAndMakeVisible(&fileComp3);

    //Call constructors for fileComponent
    /*for(int i = 0; i < numFiles; i++)
        fileComponents.push_back(std::make_unique<fileComponent>());
    */
    
    //addAndMakeVisible
    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
        addAndMakeVisible(it->get());

    globalTransportSource = std::make_shared<juce::AudioTransportSource>();
    globalTransportSource->addChangeListener(this);*/
    //Add listeners
    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
    {
        it->get()->transportSource.addChangeListener (this);
        it->get()->thumbnail.addChangeListener (this);
    }*/
    //startTimer(40);
    
    addAndMakeVisible(&fileGUI);
    addAndMakeVisible(&fileGUI2);
    
    formatManager.registerBasicFormats();
    
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
    fileAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    fileAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixer.addInputSource(&fileAudio, false);
    mixer.addInputSource(&fileAudio2, false);


}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixer.getNextAudioBlock(bufferToFill);
    //fileAudio.getNextAudioBlock(bufferToFill);
    //fileAudio2.getNextAudioBlock(bufferToFill);

    /*if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock (bufferToFill);*/
    //fileComp1.getNextAudioBlock(bufferToFill);
    //fileComp2.getNextAudioBlock(bufferToFill);
    //fileComp3.getNextAudioBlock(bufferToFill);
    /*for(int i = 0; i < 5; i++)
    {
        try{
            fileComponents.at(i)->getNextAudioBlock(bufferToFill);
        }
        catch(std::exception e)
        {
            DBG(e.what());
            DBG("Get next audio block");
        }
    }*/
    

    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
    {
        auto curr = it->get();
        curr->getNextAudioBlock(bufferToFill);
    }*/

    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
    {
        auto curr = it->get();
        if(curr->state == fileComponent::Playing)
        {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource> (curr->reader, true);
            //Set the transport source
           // transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
            globalTransportSource->setSource(newSource.get(), 0, nullptr, curr->reader->sampleRate);
            curr->getNextAudioBlock(bufferToFill);
            
        }
    }*/
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    //fileComp1.releaseResources();
    //fileComp2.releaseResources();
    //fileComp3.releaseResources();
    /*for(int i = 0; i < 5; i++)
    {
        fileComponents.at(i)->releaseResources();
    }*/
    
    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
    {
        it->get()->releaseResources();
        //(*it)->releaseResources();
    }*/
    fileAudio.releaseResources();
    fileAudio2.releaseResources();
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

    fileGUI.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
    fileGUI2.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
}

void MainComponent::timerCallback()
{
    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
        it->get()->repaint();*/
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
    {
        if (source == &((*it)->thumbnail))
            it->get()->thumbnailChanged();
    }*/
    
}
