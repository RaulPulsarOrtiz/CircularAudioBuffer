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
    Slider sldrDelayTime, sldrDelayGain, sldrFreqCutoff;
    Label delayTimeLabel, delayGainLabel, freqCutoffLabel;
    ComboBox filterTypeMenu;
    TextButton filterOnOffButton{ "Off" };
    bool filterActive = false;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, filterTypeMenuAttachment, filterOnOffAttachment, filterCutoffAttachment;

   // enum class FilterButtonState
   // {
   //     Off,
   //     On
   // };
   //
   // FilterButtonState filterButtonState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessorEditor)
};
