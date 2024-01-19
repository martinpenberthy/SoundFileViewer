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
    
    addAndMakeVisible(buttonOpenChooser);
    buttonOpenChooser.setButtonText("Add file");
    
    buttonOpenChooser.onClick = [this]
    {
        std::cout << "openButton clicked" << std::endl;
        myChooser = std::make_unique<juce::FileChooser> ("Please select the .wav you want to load...",
                                                         juce::File{},
                                                         "*.wav");
        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems | juce::FileBrowserComponent::canSelectDirectories;
        

        myChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
        {
            std::cout << "chooser" << std::endl;
            auto result = chooser.getResult();
            auto results = chooser.getResults();
            
            //User picked a directory
            if(result.isDirectory())
            {
                //DBG("No directories yet");
                std::cout << "No directories yet" << std::endl;
                int numFiles = result.getNumberOfChildFiles(20);
                //auto array = result.getc
            }
            else if(results.size() == 1) //User picked one file
            {
                std::cout << "one file picked" << std::endl;
                    
                juce::URL audioURL = juce::URL{*results.data()};
                //Make a fileComponent and add it to the vector
                filesVec.push_back(makeNewFileComponent(audioURL));
            }
            else if (results.size() > 1 && results.size() <= 20) //User picked multiple files
            {
                for(int i = 0; i < results.size(); i++)
                {
                    juce::File temp = results[i];
                    juce::URL audioURL = juce::URL{temp};
                
                    filesVec.push_back(makeNewFileComponent(audioURL));
                }
            }
            else if(results.size() > 20)
            {
                /*auto alertWindow = juce::AlertWindow("Load Error", "Too many files! (20 max)", juce::MessageBoxIconType::WarningIcon, this);
                
                addAndMakeVisible(alertWindow);*/
            }
            resized();
            prepareToPlay(myBlockSize, mySampleRate);
        });
    };

    addAndMakeVisible(comboWindowType);
    comboWindowType.addItem("Hann", 1);
    comboWindowType.addItem("Flat Top", 2);
    comboWindowType.setSelectedId(1);
    
    comboWindowType.onChange = [this] {
        for(int i = 0; i < filesVec.size(); i++)
        {
            if(filesVec[i] != nullptr)
            {
                //Make tr
                auto func = juce::dsp::WindowingFunction<float>::WindowingMethod::triangular;
                
                switch(comboWindowType.getSelectedId())
                {
                    case 1:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
                        break;
                    case 2:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::flatTop;
                        break;
                    default:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
                        break;
                        
                };
                filesVec[i]->getFileComponentGUI()->FFTDisplay.setWindowingFunction(func);
            }
        }
    };
    
    //addAndMakeVisible(file1.getFileComponentGUI());
    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 700);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

fileComponent* MainComponent::makeNewFileComponent(juce::URL audioURL)
{
    //Make fileComponent object
    auto fc = new fileComponent();
    //Load the chosen file
    fc->loadFile(audioURL);
    //Make the GUI visible
    addAndMakeVisible(fc->getFileComponentGUI());
    
    return fc;
}



//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    std::cout << "prepareToPlay" << std::endl;
    
    //fileAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
    //fileAudio2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mySampleRate = sampleRate;
    myBlockSize = samplesPerBlockExpected;
    /*
    if(file1 != nullptr)
    {
        auto tempAudio = file1->getFileComponentAudio();
        tempAudio->prepareToPlay(samplesPerBlockExpected, sampleRate);
        mixer.addInputSource(tempAudio, false);
        mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }*/
    //mixer.addInputSource(&fileAudio, false);
    //mixer.addInputSource(&fileAudio2, false);


    for(int i = 0; i < filesVec.size(); i++)
    {
        if(filesVec[i] != nullptr)
        {
            auto tempAudio = filesVec[i]->getFileComponentAudio();
            tempAudio->prepareToPlay(samplesPerBlockExpected, sampleRate);
            
            mixer.addInputSource(tempAudio, false);
            mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
        }
    }
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

    //auto tempAudio = file1->getFileComponentAudio();
    //tempAudio->releaseResources();
    
    for(int i = 0; i < filesVec.size(); i++)
    {
        if(filesVec[i] != nullptr)
        {
            auto tempAudio = filesVec[i]->getFileComponentAudio();
            tempAudio->releaseResources();
        }
    }
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
    auto const container = area;
    
    auto heightFile1 = 100;
    auto marginFile1 = 7;
    
    

    /*for(it = fileComponents.begin(); it != fileComponents.end(); it++)
        it->get()->setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));*/

    buttonOpenChooser.setBounds(area.removeFromTop(container.proportionOfHeight(0.15f)).removeFromLeft(container.proportionOfWidth(0.15f)).reduced(marginFile1));
    comboWindowType.setBounds(area.removeFromRight(container.proportionOfWidth(0.15f)));
    
    /*if(file1 != nullptr)
    {
        auto tempGUI = file1->getFileComponentGUI();
        tempGUI->setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
    }*/
    
    for(int i = 0; i < filesVec.size(); i++)
    {
        if(filesVec[i] != nullptr)
        {
            auto tempGUI = filesVec[i]->getFileComponentGUI();
            tempGUI->setBounds(area.removeFromTop(heightFile1).reduced(marginFile1));
        }
    }
}
