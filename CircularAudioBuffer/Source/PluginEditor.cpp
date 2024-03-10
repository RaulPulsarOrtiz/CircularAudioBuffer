/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularAudioBufferAudioProcessorEditor::CircularAudioBufferAudioProcessorEditor(CircularAudioBufferAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(1000, 300);

    sldrDelayTime.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
   // sldrDelayTime.setRange(0.0, 1000.0, 1.0);
    sldrDelayTime.setTextValueSuffix("ms");
    sldrDelayTime.addListener(this);
    addAndMakeVisible(sldrDelayTime);

    delayTimeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DELAYTIME", sldrDelayTime);

    sldrDelayGain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
   // sldrDelayGain.setRange(0.0, 1.1, 0.01);
    sldrDelayGain.addListener(this);
    addAndMakeVisible(sldrDelayGain);

    delayFeedbackAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYFEEDBACK", sldrDelayGain);

    delayTimeLabel.setText("Delay Time", dontSendNotification);
    delayTimeLabel.attachToComponent(&sldrDelayTime, false);
    delayTimeLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(sldrDelayTime);

    delayGainLabel.setText("Feedback Gain", dontSendNotification);
    delayGainLabel.setJustificationType(Justification::centredBottom);
    delayGainLabel.attachToComponent(&sldrDelayGain, false);
    addAndMakeVisible(delayGainLabel);

    filterTypeMenu.addItem("Lowpass", 1);
    filterTypeMenu.addItem("HighPass", 2);
    filterTypeMenu.setText("Filter Type:", dontSendNotification);
    filterTypeMenu.addListener(this);
    addAndMakeVisible(filterTypeMenu);

    filterTypeMenuAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "FILTERTYPEMENU", filterTypeMenu);
    // filterOnOffButton.onClick() = [this] { audioProcessor.filterIsOn(); }; //Instead to add a Button::Listener and use the pure virtual buttonClicked() I use this labda function which return a function just when the button is clicked

    filterOnOffButton.setToggleState(false, NotificationType::dontSendNotification);
    filterOnOffButton.setButtonText("Off");
    filterOnOffButton.setColour(TextButton::buttonColourId, Colours::red);
    filterOnOffButton.addListener(this);
    addAndMakeVisible(filterOnOffButton);

    filterOnOffAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "FILTERONOFF", filterOnOffButton);

    sldrFreqCutoff.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
   // sldrFreqCutoff.setRange(500.0, 20000.0, 1.0);
    sldrFreqCutoff.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 55, 20);
    sldrFreqCutoff.setTextValueSuffix("Hz");
    sldrFreqCutoff.addListener(this);
    addAndMakeVisible(sldrFreqCutoff);

    filterCutoffAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FILTERCUTOFF", sldrFreqCutoff);

    freqCutoffLabel.setText("Freq Cutoff", dontSendNotification);
    freqCutoffLabel.attachToComponent(&sldrFreqCutoff, false);
    freqCutoffLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(freqCutoffLabel);
}

CircularAudioBufferAudioProcessorEditor::~CircularAudioBufferAudioProcessorEditor()
{
}

//==============================================================================
void CircularAudioBufferAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
   
 
}

void CircularAudioBufferAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
  
    auto area = getBounds();
    sldrDelayTime.setBounds(100, 50, 200, 200);
    sldrDelayGain.setBounds(300, 50, 200, 200);
    filterOnOffButton.setBounds(700, 50, 50, 30);
    filterTypeMenu.setBounds(700, 90, 100, 30);
    sldrFreqCutoff.setBounds(800, 55, 90, 90);
}


void CircularAudioBufferAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
//    if (slider == &sldrDelayTime)
//    {
//  //      audioProcessor.setDelayTime(sldrDelayTime.getValue());
//    }
//
//    else if (slider == &sldrDelayGain)
//    {
//   //     audioProcessor.setDelayGain(sldrDelayGain.getValue());
//    }
//
//    else if (slider == &sldrFreqCutoff)
//    {
//    //    audioProcessor.setFreqCutoff(sldrFreqCutoff.getValue());
//    }
}

void CircularAudioBufferAudioProcessorEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &filterTypeMenu)
    {
        if (filterTypeMenu.getSelectedId() == 1) //LPF
        {
            audioProcessor.setFilterType(audioProcessor.LowPass);
            
        }

        else if (filterTypeMenu.getSelectedId() == 2) //HPF
        {
           audioProcessor.setFilterType(audioProcessor.HighPass);
          // sldrFreqCutoff.setRange(20.0, 10000.0, 1.0);  
        }
    }
}

void CircularAudioBufferAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &filterOnOffButton)
    {
        filterOnOffButton.setClickingTogglesState(true); 
       // filterButtonState == FilterButtonState::On;  //This two lines are saying the same. Hopefully if it works I can use just the enu, 'filterButtonState' insted of ToggleState

        if (filterOnOffButton.getToggleState() == true)
        {
     //       audioProcessor.filterIsOn(filterOnOffButton.getToggleState());
            filterOnOffButton.setButtonText("On");
            filterOnOffButton.setColour(TextButton::buttonColourId, Colours::green);
        }
        else if (filterOnOffButton.getToggleState() == false)
        {
         //   audioProcessor.filterIsOn(filterOnOffButton.getToggleState());
            filterOnOffButton.setButtonText("Off");
            filterOnOffButton.setColour(TextButton::buttonColourId, Colours::red);
        }
    }
}
