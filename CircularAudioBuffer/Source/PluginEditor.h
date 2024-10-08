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

/** Class to take the knob images strip */
class FilmStripSlider
{
public:
    FilmStripSlider (juce::Image* _knobStrip, int _frameCount, int _frameSize, bool _isVerticalStrip);
    ~FilmStripSlider() {}

    /** This function workout the minimum and maximum values of the slider and it shows the corresponding frame of the image strip based on the current value */
    void drawFrame(juce::Graphics& g, int x, int y, int width, int height, juce::Slider& slider);

private:
    /** This image is a strip of the different images of the slider with every different value  */
    juce::Image* knobStrip = nullptr;

    /** Number of frame values on the slider */
    int frameCount = 0; 

    /** Width of the image */
    int frameSize = 0;

    /** If that image is vertical or horizontal */
    bool isVerticalStrip = true;
};

/** Custom LookAndFeel class for delayTime and Feedback sliders */
class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel() //On the constructor we set up some default colours for different parts of the GUI
    {
        /* Remains of a past GUI */
        //   setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::azure); //Line of the Slider
        //   setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::goldenrod); //Line of the slider from 0 to the current value
        //   setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack); //Dot of the current value
        // setColour(juce::Slider::trackColourId, juce::Colours::cornflowerblue);
    }
    ~OtherLookAndFeel() {}

    /** Get Image (binary data) from memory
    * @param BinaryData::BinaryData::BOS_knob_DIVA_52x52_128f_png is the Binary Data of image for the delayTime knob and feedback slider (called sldrDelayTime and in the code)
    * @param BinaryData::BOS_knob_DIVA_52x52_128f_pngSize is the size of the frame of each slider image in the strip
    */
    Image knobFeedbackDelayStrip = ImageCache::getFromMemory(BinaryData::BOS_knob_DIVA_52x52_128f_png, BinaryData::BOS_knob_DIVA_52x52_128f_pngSize);
        
    /** This funcion draws the slider image for the delayTime and feedback, instead of drawing a standard JUCE slider */
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle,
    const float rotaryEndAngle, juce::Slider&) override; // (x, y, width, height) are passing from the slider's resize() function. 
                                                         //sliderPost is where the slider is from 0 to 1.
                                                         // rotaryAngle is the angle in radiants from the center of the slider to the start position or to the end position
};

/** Custom LookAndFeel class for filter sliders */
class FilterLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FilterLookAndFeel() {}
    ~FilterLookAndFeel() {}

    /** Get Image (binary data) from memory 
    * @param BinaryData::Emu_Drummulator_png is the Binary Data of image for the Filter Cutoff slider (called sldrFreqCutoff and sldrDelayGain in the code)
    * @param BinaryData::Emu_Drummulator_pngSize is the size of the frame of each slider image in the strip
    */
    Image knobFilterStrip = ImageCache::getFromMemory(BinaryData::Emu_Drummulator_png, BinaryData::Emu_Drummulator_pngSize); 

    /** This funcion draws the slider image for the filter, instead of drawing a standard JUCE slider */
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle,
        const float rotaryEndAngle, juce::Slider&) override; // (x, y, width, height) are passing from the slider's resize() function. 
                                                             //sliderPost is where the slider is from 0 to 1.
                                                             // rotaryAngle is the angle in radiants from the center of the slider to the start position or to the end position
};

class CircularAudioBufferAudioProcessorEditor : public juce::AudioProcessorEditor,
                                                public Button::Listener
{
public:
    CircularAudioBufferAudioProcessorEditor (CircularAudioBufferAudioProcessor&);
    ~CircularAudioBufferAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;

    /** Set the bounds of the components in the GUI*/
    void resized() override;

   /** Listener to activate Filter Button */
    void buttonClicked(Button*) override;

    /** Takes the value of the feedback to light on red if the value is more than 1 (Dangerous feedback loop) */
    /** Set the colour of the Feedback Slider Label. If the value of the slider is more than 1, the Label turns red to warn about the incresing feedback loop*/
    void updateSliderColour(float value);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CircularAudioBufferAudioProcessor& audioProcessor;
   
    /**Custom Look&Feel for 'Delay Time' and 'Feedback' sliders */
    OtherLookAndFeel otherLookAndFeel;

    /**Custom Look&Feel for 'Filter' slider */
    FilterLookAndFeel filterLookAndFeel;

    /** Declaration of Components */
    Slider sldrDelayTime, sldrDelayGain, sldrFreqCutoff;
    Label delayTimeLabel, delayGainLabel, freqCutoffLabel;
    ComboBox filterTypeMenu, syncBpmMenu;
    TextButton filterOnOffButton{ "Off" };

    /** Variables refering to the post-feedback filter. Used on the choice of filter type*/
    int filterTypeInt = 0; 
    enum FilterType
    {
        LowPass,
        BandPass,
        HighPass
    };
    FilterType filterTypeEnum;
    String filterTypeName;

    /** Variable to choose fixed delayTime menu*/
    int syncTimeIndexMenu = 0;

    /** ValueTreeState Attachments */
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, filterCutoffAttachment; //These attachment alwats below the declaration of the Sldr
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeMenuAttachment, syncBpmMenuAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> filterOnOffAttachment;

    /** Wood image for background GUI*/
    Image background;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessorEditor)
};
