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

class FilmStripSlider
{
public:
    FilmStripSlider (juce::Image* _knobStrip, int _frameCount, int _frameSize, bool _isVerticalStrip);
    void drawFrame(juce::Graphics& g, int x, int y, int width, int height, juce::Slider& slider);

private:
    juce::Image* knobStrip = nullptr;
    int frameCount = 0; //Number of values on the knob
    int frameSize = 0; //width of the image
    bool isVerticalStrip = true;
};

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel() //On the constructor we set up some default colours for different parts of the GUI
    {
        //   setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::azure); //Line of the Slider
        //   setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::goldenrod); //Line of the slider from 0 to the current value
        //   setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack); //Dot of the current value
          // setColour(juce::Slider::trackColourId, juce::Colours::cornflowerblue);
    }
       Image knobFeedbackDelayStrip = ImageCache::getFromMemory(BinaryData::BOS_knob_DIVA_52x52_128f_png, BinaryData::BOS_knob_DIVA_52x52_128f_pngSize);
     //   FilmStripSlider knobFeedbackDelay = FilmStripSlider(&knobFeedbackDelayStrip, 110, 80, true);
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle,
        const float rotaryEndAngle, juce::Slider&) override; // (x, y, width, height) are passing from the slider's resize() function. 
                                                             //sliderPost is where the slider is from 0 to 1.
                                                             // rotaryAngle is the angle in radiants from the center of the slider to the start position or to the end position

  //  void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
  //      bool, bool isButtonDown) override;
   
};

class FilterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FilterLookAndFeel() {}
    Image knobFilterStrip = ImageCache::getFromMemory(BinaryData::Emu_Drummulator_png, BinaryData::Emu_Drummulator_pngSize);
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle,
        const float rotaryEndAngle, juce::Slider&) override; // (x, y, width, height) are passing from the slider's resize() function. 
                                                             //sliderPost is where the slider is from 0 to 1.
                                                             // rotaryAngle is the angle in radiants from the center of the slider to the start position or to the end position
};

class CircularAudioBufferAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                public Slider::Listener,
                                             //   public ComboBox::Listener,
                                                public Button::Listener
{
public:
    CircularAudioBufferAudioProcessorEditor (CircularAudioBufferAudioProcessor&);
    ~CircularAudioBufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(Button*) override;
    void updateSliderColour(float value);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CircularAudioBufferAudioProcessor& audioProcessor;
    OtherLookAndFeel otherLookAndFeel;
    FilterLookAndFeel filterLookAndFeel;
    Slider sldrDelayTime, sldrDelayGain, sldrFreqCutoff;
    Label delayTimeLabel, delayGainLabel, freqCutoffLabel;
    ComboBox filterTypeMenu;
    TextButton filterOnOffButton{ "Off" };
    bool filterActive = false;

    int filterTypeInt = 0;
   
    enum FilterType
    {
        LowPass,
        BandPass,
        HighPass
    };
    FilterType filterTypeEnum;
    String filterTypeName;
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

    Image background;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessorEditor)
};
