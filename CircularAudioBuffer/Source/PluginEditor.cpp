/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//void OtherLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
//    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) 
//{
//    auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
//    auto centreX = (float)x + (float)width * 0.5f;
//    auto centreY = (float)y + (float)height * 0.5f;
//    auto rx = centreX - radius;
//    auto ry = centreY - radius;
//    auto rw = radius * 2.0f;
//    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
//
//    // fill
//    g.setColour(juce::Colours::orange);
//    g.fillEllipse(rx, ry, rw, rw);
//
//    // outline
//    g.setColour(juce::Colours::red);
//    g.drawEllipse(rx, ry, rw, rw, 1.0f);
//
//    juce::Path p;
//    auto pointerLength = radius * 0.33f;
//    auto pointerThickness = 2.0f;
//    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
//    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
//
//    // pointer
//    g.setColour(juce::Colours::yellow);
//    g.fillPath(p);
//}
//
//void OtherLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
//    bool, bool isButtonDown)
//{
//    auto buttonArea = button.getLocalBounds();
//    auto edge = 4;
//
//    buttonArea.removeFromLeft(edge);
//    buttonArea.removeFromTop(edge);
//
//    // shadow
//    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
//    g.fillRect(buttonArea);
//
//    auto offset = isButtonDown ? -edge / 2 : -edge;
//    buttonArea.translate(offset, offset);
//
//    g.setColour(backgroundColour);
//    g.fillRect(buttonArea);
//}
    
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
    sldrDelayTime.setLookAndFeel(&otherLookAndFeel);
    addAndMakeVisible(sldrDelayTime);

    delayTimeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DELAYTIME", sldrDelayTime);

    sldrDelayGain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
   // sldrDelayGain.setRange(0.0, 1.1, 0.01);
    sldrDelayGain.setLookAndFeel(&otherLookAndFeel);
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
    filterTypeMenu.addItem("Bandpass", 2);
    filterTypeMenu.addItem("HighPass", 3);
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
   // setLookAndFeel(nullptr);
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
      if (slider == &sldrDelayGain)
    {
          if (sldrDelayGain.getValue() >= 1.0)
          {
              sldrDelayGain.setColour(juce::Slider::thumbColourId, juce::Colours::red);
          //    sldrDelayGain.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red);
          }
          else
          {
              sldrDelayGain.setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
            //  sldrDelayGain.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::azure);
           //   sldrDelayGain.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::azure);
          }
    }
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

        else if (filterTypeMenu.getSelectedId() == 2) //BPF
        {
            audioProcessor.setFilterType(audioProcessor.BandPass);
            // sldrFreqCutoff.setRange(20.0, 10000.0, 1.0);  
        }

        else if (filterTypeMenu.getSelectedId() == 3) //HPF
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
