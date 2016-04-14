/*
 ==============================================================================
 
 This file was auto-generated!
 
 ==============================================================================
 */

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <aubio/aubio.h>
#include "FreqMap.h"

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class MainContentComponent   :  public AudioAppComponent,
private Button::Listener,
private TextEditor::Listener,
private ComboBox::Listener,
private MidiInputCallback,
private MidiKeyboardStateListener,
private Timer
{
public:
    //==============================================================================
    MainContentComponent()
    : lastInputIndex (0),
    isAddingFromMidiInput (false),
    keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible (startCalibrationButton);
        startCalibrationButton.setButtonText ("Start Caribration");
        startCalibrationButton.addListener (this);
        
        addAndMakeVisible (oscStatusLabel);
        oscStatusLabel.setColour (Label::backgroundColourId, Colours::black);
        oscStatusLabel.setColour (Label::textColourId, Colours::white);
        oscStatusLabel.setJustificationType (Justification::centred);
        oscStatusLabel.setText ("OSCtoCV Calibrator", dontSendNotification);
        
        addAndMakeVisible (tunerLabel);
        tunerLabel.setColour (Label::backgroundColourId, Colours::black);
        tunerLabel.setColour (Label::textColourId, Colours::white);
        tunerLabel.setJustificationType (Justification::centred);
        
        addAndMakeVisible (remoteAddressText);
        remoteAddressText.setTextToShowWhenEmpty("127.0.0.1", Colours::black);
        remoteAddressText.setText("127.0.0.1");
        remoteAddressText.addListener (this);
        
        addAndMakeVisible (remotePortText);
        remotePortText.setTextToShowWhenEmpty("8000", Colours::black);
        remotePortText.setText("8000");
        remotePortText.addListener (this);
        
        addAndMakeVisible (addressPattern);
        addressPattern.setTextToShowWhenEmpty(oscAddress, Colours::black);
        addressPattern.setText(oscAddress);
        addressPattern.addListener (this);
        
        addAndMakeVisible (intModeButton);
        intModeButton.setButtonText("int");
        intModeButton.setColour (TextButton::buttonOnColourId, Colour (0xfff4f3f3));
        intModeButton.setToggleState(true, dontSendNotification);
        intModeButton.addListener (this);
        
        addAndMakeVisible (maxIntRange);
        maxIntRange.setTextToShowWhenEmpty(maxIntText, Colours::black);
        maxIntRange.setText(maxIntText);
        maxIntRange.addListener (this);
        
        addAndMakeVisible (saveCalibrationDataButton);
        saveCalibrationDataButton.setButtonText ("Save Calibration Data");
        saveCalibrationDataButton.addListener (this);
        
        addAndMakeVisible (midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
        const StringArray midiInputs (MidiInput::getDevices());
        midiInputList.addItemList (midiInputs, 1);
        midiInputList.addListener (this);
        
        addAndMakeVisible (keyboardComponent);
        keyboardState.addListener (this);
        
        setSize (320, 400);
        
        // find the first enabled device and use that by default
        for (int i = 0; i < midiInputs.size(); ++i)
        {
            if (deviceManager.isMidiInputEnabled (midiInputs[i]))
            {
                setMidiInput (i);
                break;
            }
        }
        
        // if no enabled devices were found just use the first one in the list
        if (midiInputList.getSelectedId() == 0)
        {
            setMidiInput (0);
        }
        
        AudioDeviceManager::AudioDeviceSetup deviceSetup;
        deviceSetup.sampleRate = SAMPLE_RATE;
        deviceSetup.bufferSize = BUF_SIZE;
        
        setOpaque (true);
        setAudioChannels (1, 0);
        
        // Create pitch detect object
        inputBuf = new_fvec(HOP_SIZE);
        outputBuf = new_fvec(HOP_SIZE);
        
        o = new_aubio_pitch ((char *)"fcomb", BUF_SIZE, HOP_SIZE, SAMPLE_RATE);
        aubio_pitch_set_silence(o, -90);
        aubio_pitch_set_tolerance(o, 0.95);
        
        noteIdx = currentFloatValue = currentIntValue = 0;
        startCalibration = false;
        
        startTimer (2); // start TimerCallback;
    }
    
    ~MainContentComponent()
    {
        startCalibrationButton.removeListener (this);
        saveCalibrationDataButton.removeListener (this);
        remoteAddressText.removeListener (this);
        remotePortText.removeListener (this);
        addressPattern.removeListener (this);
        intModeButton.removeListener (this);
        maxIntRange.removeListener (this);
        
        keyboardState.removeListener (this);
        deviceManager.removeMidiInputCallback (MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
        midiInputList.removeListener (this);
        
        del_aubio_pitch (o);
        del_fvec (outputBuf);
        del_fvec (inputBuf);
        aubio_cleanup ();
        
        shutdownAudio();
    }
    
    //=======================================================================
    void prepareToPlay (int /*samplesPerBlockExpected*/, double /*newSampleRate*/) override
    {
        
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        const float * sampleData = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
        
        for (int i = 0; i < bufferToFill.numSamples; ++i)
        {
            inputBuf->data[i] = sampleData[i];
        }
        
        aubio_pitch_do (o, inputBuf, outputBuf);
        currentPitch = outputBuf->data[0];
        
        checkFrequencyDiff();
    }
    
    void releaseResources() override
    {
        
    }
    
    //=======================================================================
    void paint (Graphics& g) override
    {
        //g.fillAll (Colours::black);
    }
    
    void resized() override
    {
        startCalibrationButton.setBounds (10, 10, 300, 40);
        oscStatusLabel.setBounds (10, 60, 300, 30);
        tunerLabel.setBounds (10, 100, 300, 30);
        remoteAddressText.setBounds(10, 140, 150, 20);
        remotePortText.setBounds (170, 140, 140, 20);
        addressPattern.setBounds (10, 170, 150, 20);
        intModeButton.setBounds (170, 170, 40, 20);
        maxIntRange.setBounds (220, 170, 90, 20);
        saveCalibrationDataButton.setBounds (10, 200, 300, 40);
        midiInputList.setBounds (10, 250, 300, 30);
        keyboardComponent.setBounds (10, 290, 300, 100);
    }
    
    void setMidiInput (int index)
    {
        const StringArray list (MidiInput::getDevices());
        
        deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
        
        const String newInput (list[index]);
        
        if (! deviceManager.isMidiInputEnabled (newInput))
        {
            deviceManager.setMidiInputEnabled (newInput, true);
        }
        
        deviceManager.addMidiInputCallback (newInput, this);
        midiInputList.setSelectedId (index + 1, dontSendNotification);
        
        lastInputIndex = index;
    }
    
    void comboBoxChanged (ComboBox* box) override
    {
        if (box == &midiInputList)
        {
            setMidiInput (midiInputList.getSelectedItemIndex());
        }
    }
    
    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override
    {
        const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
        keyboardState.processNextMidiEvent (message);
    }
    
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
        
        sender.send (oscAddress, (float)calibMap[m.getNoteNumber()]);
    }
    
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        if (! isAddingFromMidiInput)
        {
            MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber));
        }
    }
    
    void buttonClicked (Button* button) override
    {
        if (button == &startCalibrationButton)
        {
            sender.disconnect();
            
            remotePort = remotePortText.getText();
            oscAddress = addressPattern.getText();
            maxIntText = maxIntRange.getText();
            
            if (! sender.connect (remoteAddressText.getText(), remotePort.getIntValue()))
            {
                oscStatusLabel.setText ("Error: could not connect to UDP", dontSendNotification);
                return;
                
            } else if (!startCalibration) {
                
                startCalibrationButton.setButtonText ("Stop Caribration");
                oscStatusLabel.setText ("Calibrationing", dontSendNotification);
                
                noteIdx = currentFloatValue = currentIntValue = 0;
                startCalibration = true;
                
            } else {
                
                noteIdx = currentFloatValue = currentIntValue = 0;
                startCalibration = false;
                startCalibrationButton.setButtonText ("Start Caribration");
                oscStatusLabel.setText ("OSCtoCV Calibrator", dontSendNotification);
                return;
            }
            
        } else if (button == &intModeButton) {
            
            if (!intCalibMode)
            {
                intModeButton.setColour (TextButton::buttonOnColourId, Colour (0xff00cc88));
                
                intCalibMode = true;
                
            } else {
                
                intModeButton.setColour (TextButton::buttonOnColourId, Colour (0xfff4f3f3));
                
                intCalibMode = false;
            }
            
        } else if (button == &saveCalibrationDataButton) {
            
            FileChooser ChooseFile("Save Calibration Data As.", File::getSpecialLocation(File::userHomeDirectory), "*.txt");
            
            if(ChooseFile.browseForFileToSave(true))
            {
                File calibDataText = ChooseFile.getResult().withFileExtension("txt");
                calibDataText.create();
                
                for (int i = 0; i < 128; ++i)
                {
                    std::stringstream calibValue;
                    calibValue << std::fixed << std::setprecision(9) << calibMap[i];
                    
                    calibDataText.appendText(calibValue.str());
                    
                    if (i != 127)
                    {
                        calibDataText.appendText(",\n");
                    }
                    
                }
            }
            
        }
    }
    
    void textEditorReturnKeyPressed (TextEditor &editor) override
    {
        sender.disconnect();
        
        remotePort = remotePortText.getText();
        oscAddress = addressPattern.getText();
        
        if (! sender.connect (remoteAddressText.getText(), remotePort.getIntValue()))
        {
            oscStatusLabel.setText ("Error: could not connect to UDP", dontSendNotification);
            
        } else {
            
            oscStatusLabel.setText ("UDP Connection succeeded", dontSendNotification);
        }
    }
    
    void sendOSCMessage()
    {
        if (intCalibMode)
        {
            sender.send (oscAddress, currentIntValue);
            currentIntValue += 1;
            
            if (noteIdx == 128 || currentIntValue > maxIntText.getIntValue())
            {
                oscStatusLabel.setText ("Calibration complete", dontSendNotification);
                startCalibration = false;
            }
            
        } else {
            
            sender.send (oscAddress, (float)currentFloatValue);
            currentFloatValue += 0.000015259021896696;
            
            if (noteIdx == 128 || currentFloatValue > 1.0f)
            {
                
                oscStatusLabel.setText ("Calibration complete", dontSendNotification);
                startCalibration = false;
            }
        }
        
    }
    
    void checkFrequencyDiff()
    {
        float distance = std::abs(freqMap[noteIdx] - currentPitch);
        static float diff = 1.005;
        
        if (distance < log(diff))
        {
            if (intCalibMode)
            {
                calibMap[noteIdx++] = currentIntValue;
                
            } else {
                
                calibMap[noteIdx++] = currentFloatValue;
            }
            
            diff += 0.008;
            
            if (noteIdx == 105)
            {
                stopTimer ();
                startTimer (28);
            }
        }
    }
    
    void timerCallback() override
    {
        tunerLabel.setText (String (currentPitch, 2), dontSendNotification);
        
        if (startCalibration)
        {
            String calibrationStatus;
            
            if (intCalibMode)
            {
                calibrationStatus << "MIDI :: " << noteIdx << "    OSC :: " << String (currentIntValue);
                
            } else {
                
                calibrationStatus << "MIDI :: " << noteIdx << "    OSC :: " << String (currentFloatValue, 4);
            }
            
            oscStatusLabel.setText (calibrationStatus, dontSendNotification);
            
            sendOSCMessage();
        }
    }
    
    
private:
    //==============================================================================
    
    enum
    {
        BUF_SIZE = 8192,
        HOP_SIZE = 512,
        SAMPLE_RATE= 48000
    };
    
    float currentFloatValue = 0.0f;
    uint16_t currentIntValue = 0;
    float currentPitch;
    uint8_t noteIdx = 0;
    
    bool  startCalibration = false;
    bool  intCalibMode = false;
    
    juce::String remotePort;
    juce::String oscAddress = "/fader1";
    juce::String maxIntText = "65535";
    
    int  lastInputIndex;
    bool isAddingFromMidiInput;
    
    OSCSender sender;
    
    // =============================================================================
    // Gui object
    TextButton startCalibrationButton;
    TextButton saveCalibrationDataButton;
    TextButton intModeButton;
    Label oscStatusLabel;
    Label tunerLabel;
    TextEditor remoteAddressText;
    TextEditor remotePortText;
    TextEditor addressPattern;
    TextEditor maxIntRange;
    ComboBox midiInputList;
    Label midiInputListLabel;
    
    AudioDeviceManager deviceManager;
    
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    
    // =============================================================================
    // Aubio Pitch detection object
    aubio_pitch_t *o;
    fvec_t *inputBuf;
    fvec_t *outputBuf;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
