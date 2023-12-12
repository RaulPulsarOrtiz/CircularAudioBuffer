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
class CircularAudioBufferAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CircularAudioBufferAudioProcessorEditor (CircularAudioBufferAudioProcessor&);
    ~CircularAudioBufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void getDelayTimeValue();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CircularAudioBufferAudioProcessor& audioProcessor;
    Slider sldrDelayTime, sldrDelayGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessorEditor)
};
