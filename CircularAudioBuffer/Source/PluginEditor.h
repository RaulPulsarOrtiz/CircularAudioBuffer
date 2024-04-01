/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::azure); //Line of the Slider
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::goldenrod); //Line of the slider from 0 to the current value
        setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack); //Dot of the current value
       // setColour(juce::Slider::trackColourId, juce::Colours::cornflowerblue);
    }
   void drawRotarySlider(juce::Graphics & g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, 
       const float rotaryEndAngle, juce::Slider&) override; // (x, y, width, height) are passing from the slider's resize() function. 
                                                            //sliderPost is where the slider is from 0 to 1.
                                                            // rotaryAngle is the angle in radiants from the center of the slider to the start position or to the end position

 //  void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
 //      bool, bool isButtonDown) override;
    
};

class CircularAudioBufferAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                public Slider::Listener,
                                                public ComboBox::Listener,
                                                public Button::Listener
{
public:
    CircularAudioBufferAudioProcessorEditor (CircularAudioBufferAudioProcessor&);
    ~CircularAudioBufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(Slider* slider) override;
    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(Button*) override;

    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CircularAudioBufferAudioProcessor& audioProcessor;
    OtherLookAndFeel otherLookAndFeel;
    Slider sldrDelayTime, sldrDelayGain, sldrFreqCutoff;
    Label delayTimeLabel, delayGainLabel, freqCutoffLabel;
    ComboBox filterTypeMenu;
    TextButton filterOnOffButton{ "Off" };
    bool filterActive = false;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, filterCutoffAttachment; //These attachment alwats below the declaration of the Sldr
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeMenuAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> filterOnOffAttachment;
   // enum class FilterButtonState
   // {
   //     Off,
   //     On
   // };
   //
   // FilterButtonState filterButtonState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessorEditor)
};
