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

    sldrDelayTime.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sldrDelayTime.setRange(0, 2000);
    sldrDelayTime.setTextValueSuffix("s");
    sldrDelayTime.addListener(this);
    addAndMakeVisible(sldrDelayTime);

    sldrDelayGain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sldrDelayGain.setRange(0.0, 1.1);
    sldrDelayGain.addListener(this);
    addAndMakeVisible(sldrDelayGain);
    
}

void CircularAudioBufferAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle <int> plugInArea = getBounds();
    auto middleAreaVertical = plugInArea.getHeight();
   // Rectangle<int> xMiddleAreaVertical = middleAreaVertical.getX();

   // auto thirdOfAreaHorizontal = plugInArea.getWidth() / 3;
    sldrDelayTime.setBounds(100, 100, 200, 200);
   // sldrDelayTime.setBounds()
    sldrDelayGain.setBounds(300, 100, 200, 200);
}


void CircularAudioBufferAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &sldrDelayTime)
    {
        audioProcessor.setDelayTime(sldrDelayTime.getValue());
    }
}

