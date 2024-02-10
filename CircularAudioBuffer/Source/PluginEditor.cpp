/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularAudioBufferAudioProcessorEditor::CircularAudioBufferAudioProcessorEditor (CircularAudioBufferAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 300);

    sldrDelayTime.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sldrDelayTime.setRange(0.0, 1000.0, 1.0);
    sldrDelayTime.setTextValueSuffix("ms");
    sldrDelayTime.addListener(this);
    addAndMakeVisible(sldrDelayTime);

    sldrDelayGain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sldrDelayGain.setRange(0.0, 1.1, 0.01);
    sldrDelayGain.addListener(this);
    addAndMakeVisible(sldrDelayGain);

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
    // filterTypeMenu.addListener(this);
    addAndMakeVisible(filterTypeMenu);
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
    filterTypeMenu.setBounds(700, 50, 100, 30);
}


void CircularAudioBufferAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &sldrDelayTime)
    {
        audioProcessor.setDelayTime(sldrDelayTime.getValue());
    }

    else if (slider == &sldrDelayGain)
    {
        audioProcessor.setDelayGain(sldrDelayGain.getValue());
    }
}

void CircularAudioBufferAudioProcessorEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &filterTypeMenu)
    {
        if (filterTypeMenu.getSelectedId() == 1) //LPF
        {
            //audioProcessor.setFilterType(audioProcessor.LowPass);
            //cutoffSldr.setValue(20000);
        }

        else if (filterTypeMenu.getSelectedId() == 2) //HPF
        {
           // audioProcessor.setFilterType(audioProcessor.HighPass);
           // cutoffSldr.setValue(20);
        }
    }
}

