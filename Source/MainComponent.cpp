#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

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
    
        
    addAndMakeVisible(&fileComp1);
    addAndMakeVisible(&fileComp2);
    addAndMakeVisible(&fileComp3);

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
    fileComp1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    fileComp2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    fileComp3.prepareToPlay(samplesPerBlockExpected, sampleRate);

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{

    /*if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock (bufferToFill);*/
    fileComp1.getNextAudioBlock(bufferToFill);
    fileComp2.getNextAudioBlock(bufferToFill);
    fileComp3.getNextAudioBlock(bufferToFill);


}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    fileComp1.releaseResources();
    fileComp2.releaseResources();
    fileComp3.releaseResources();
    
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
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    //fileComp1.setBounds(getWidth(), getHeight(), getWidth(), getHeight() / 4);
    
    auto area = getLocalBounds();
    
    auto heightFile1 = 50;
    auto marginFile1 = 7;
    fileComp1.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
    fileComp2.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
    fileComp3.setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));

}
