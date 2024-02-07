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
    
    contComponent.addAndMakeVisible(buttonOpenChooser);
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
                //int numFiles = result.getNumberOfChildFiles(juce::File::TypesOfFileToFind::findFiles, "*.wav");
                //auto array = result.getc
                
                juce::Array<juce::File> filesArray = result.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wav");
                
                for(int i = 0; i < filesArray.size(); i++)
                {
                    auto tempFile = filesArray[i];
                    juce::URL audioURL = juce::URL{tempFile};
                    //Make a fileComponent and add it to the vector
                    filesVec.push_back(makeNewFileComponent(audioURL));
                }
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

    contComponent.addAndMakeVisible(comboWindowType);
    comboWindowType.addItem("Hann", 1);
    comboWindowType.addItem("Flat Top", 2);
    comboWindowType.addItem("Rectangular", 3);
    comboWindowType.addItem("Triangular", 4);
    comboWindowType.addItem("Hamming", 5);
    comboWindowType.addItem("Blackman-Harris", 6);
    comboWindowType.addItem("Blackman", 7);
    comboWindowType.addItem("Kaiser", 8);
    
    
    comboWindowType.setSelectedId(1);
    
    comboWindowType.onChange = [this] {
        for(int i = 0; i < filesVec.size(); i++)
        {
            if(filesVec[i] != nullptr)
            {
                //Make temp
                juce::dsp::WindowingFunction<float>::WindowingMethod func;
                
                switch(comboWindowType.getSelectedId())
                {
                    case 1:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
                        break;
                    case 2:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::flatTop;
                        break;
                    case 3:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::rectangular;
                        break;
                    case 4:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::triangular;
                        break;
                    case 5:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::hamming;
                        break;
                    case 6:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::blackmanHarris;
                        break;
                    case 7:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::blackman;
                        break;
                    case 8:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::kaiser;
                        break;
                    default:
                        func = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
                        break;
                        
                };
                filesVec[i]->getFileComponentGUI()->FFTDisplay.setWindowingFunction(func);
            }
        }
    };
    
    myViewport.setViewedComponent(&contComponent, false);
    addAndMakeVisible(myViewport);
    

    
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (900, 200);
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
    contComponent.addAndMakeVisible(fc->getFileComponentGUI());
    
    return fc;
}



//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    std::cout << "prepareToPlay" << std::endl;
    
    mySampleRate = sampleRate;
    myBlockSize = samplesPerBlockExpected;

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
    g.fillAll (juce::Colours::darkgrey);

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    contComponent.setBounds(0, 0, 900, 700);
    myViewport.setBounds(0, 0, getWidth(), getHeight());
    
    auto area = contComponent.getLocalBounds();
    auto area2 = contComponent.getLocalBounds();
    area = area.removeFromTop(contComponent.proportionOfHeight(0.1f));
    area2 = area2.removeFromBottom(contComponent.proportionOfHeight(0.9f));
    auto const container = area;
    
    auto heightFile1 = 100;
    auto marginFile1 = 7;
    
    buttonOpenChooser.setBounds(area.removeFromLeft(container.proportionOfWidth(0.5f)).reduced(7));
    comboWindowType.setBounds(area.removeFromLeft(container.proportionOfWidth(0.5f)).reduced(7));
    
    for(int i = 0; i < filesVec.size(); i++)
    {
        if(filesVec[i] != nullptr)
        {
            auto tempGUI = filesVec[i]->getFileComponentGUI();
            tempGUI->setBounds(area2.removeFromTop(heightFile1).reduced(marginFile1));
        }
    }
}
