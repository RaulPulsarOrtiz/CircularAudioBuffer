/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../../../../../3 - 2021-2022 Third/SDA/Exercises/First Time/Practical05 Juce/modules/juce_core/system/juce_PlatformDefs.h"

FilmStripSlider::FilmStripSlider(juce::Image* _knobStrip, int _frameCount, int _frameSize, bool _isVerticalStrip) {
    knobStrip = _knobStrip;
    frameSize = _frameSize;
    frameCount = _frameCount;
    isVerticalStrip = _isVerticalStrip;
}

void FilmStripSlider::drawFrame(juce::Graphics& g, int x, int y, int width, int height, juce::Slider& slider) {
  //  DBG("slider min: " << slider.getMinimum() << " slider max: " << slider.getMaximun());
    //float div = (float)((slider.getMaximun() - slider.getMinimun()) / frameCount);

   // DBG("slider val: " << slider.getValue());
    //double pos + (int)((slider.getValue() + slider.getMinimum()) / div;

    const double fractRotation = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum()); //value between 0 and 1 for current amount of rotation
    int pos = (int)ceil(fractRotation * ((double)frameCount - 1.0)); // current index from 0 ---> nFrames-1

    slider.getRange();

    //if (pos > 0)
    // pos = pos - 1;

    if (width != height) /* scale */
    {
        x = (width / 2) - (height / 2);
        width = height;
    }

    if (isVerticalStrip)
    {
        g.drawImage(*knobStrip, x, y, width, height, 0, (int)(pos * frameSize), frameSize, frameSize, false);
    }
    else
    {
        g.drawImage(*knobStrip, x, y, width, height, (int)(pos * frameSize), 0, frameSize, frameSize, false);
    }
}

void OtherLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
/** Remains of apast GUI*/
//    auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f; //take the width and the height, compare which one is minor of those. which would be the diameter of the knob. Divide by two to get the radius
//    auto centreX = (float)x + (float)width * 0.5f; // X would be 0. Plus half of the width is guiven the X centre.
//    auto centreY = (float)y + (float)height * 0.5f; // Y would be 0. Plus half of the width is guiven the Y centre.
//    auto rx = centreX - radius; //It gives the beggining x position of the circle
//    auto ry = centreY - radius; //It gives the beggining y position of the circle
//    auto rw = radius * 2.0f; //The whole diameter (width)
//    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle); //SliderPos isn't going to be between 0 and 1. It is going to be moved in the valid range that is the maximun angle minus the minumun angle. So SliderPos is multiply for that valid range. And it doesn't start from 0, it star from the minumun angle value, so that is summed.
   //bool, bool isButtonDown) override;

// ignoreUnused(rotaryEndAngle);
// ignoreUnused(rotaryStartAngle);
// ignoreUnused(sliderPos);
// knobFeedbackDelay.drawFrame(g, x, y, width, height, slider);

//  Image knobFeedbackDelayStrip = ImageCache::getFromMemory(BinaryData::BOS_knob_DIVA_52x52_128f_png, BinaryData::BOS_knob_DIVA_52x52_128f_pngSize);
//  knobFeedbackDelayStrip.rescaled(width * 0.6, height, juce::Graphics::ResamplingQuality::highResamplingQuality);

//  fill
//  g.setColour(juce::Colours::orange); //Choose Color for the inner circle
//  g.fillEllipse(rx, ry, rw, rw); //Fill the ellipse. In this case a circle
 
//  g.drawImage(knobFeedbackDelayStrip, 300, 50, 265, 200, 52, 0, 50, 0, false);   

    // outline
 // g.setColour(juce::Colours::red); //Choose color for the outer line of the circle
 // g.drawEllipse(rx, ry, rw, rw, 1.0f); //Fill the line

 //   juce::Path p; //Tick inside the circule
 //   auto pointerLength = radius * 0.33f;
 //   auto pointerThickness = 2.0f;
 //   p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
 //   p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
 //
    //pointer
 //   g.setColour(juce::Colours::yellow);
 //   g.fillPath(p);

    FilmStripSlider knobFeedbackDelay = FilmStripSlider(&knobFeedbackDelayStrip, 110, 52, true);
    knobFeedbackDelay.drawFrame(g, x, y, width, height, slider);
}
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
    
void FilterLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle,
    const float rotaryEndAngle, juce::Slider& slider)
{
    FilmStripSlider knobFilterDelay = FilmStripSlider(&knobFilterStrip, 20, 83, true);
    knobFilterDelay.drawFrame(g, x, y, width, height, slider);
}
//==============================================================================
CircularAudioBufferAudioProcessorEditor::CircularAudioBufferAudioProcessorEditor(CircularAudioBufferAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(1000, 300);
    
    sldrDelayTime.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sldrDelayTime.setTextValueSuffix("ms");
    sldrDelayTime.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sldrDelayTime.setLookAndFeel(&otherLookAndFeel);
    addAndMakeVisible(sldrDelayTime);
 
    delayTimeAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "DELAYTIME", sldrDelayTime);

    sldrDelayGain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sldrDelayGain.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sldrDelayGain.setLookAndFeel(&otherLookAndFeel);
    addAndMakeVisible(sldrDelayGain);

    delayFeedbackAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYFEEDBACK", sldrDelayGain);

    sldrDelayGain.onValueChange = [this]() {
        float currentValue = sldrDelayGain.getValue();
        updateSliderColour(currentValue);
    };

    delayTimeLabel.setText("Delay Time", dontSendNotification);
    delayTimeLabel.attachToComponent(&sldrDelayTime, false);
    delayTimeLabel.setCentrePosition(297, 40);
    delayTimeLabel.setJustificationType(Justification::topLeft);
    addAndMakeVisible(sldrDelayTime);

    delayGainLabel.setText("Feedback Gain", dontSendNotification);
    delayGainLabel.attachToComponent(&sldrDelayGain, false);
    delayGainLabel.setJustificationType(Justification::topLeft);
    delayGainLabel.setCentrePosition(626, 40);
    addAndMakeVisible(delayGainLabel);

    filterTypeMenu.addItem("Lowpass", 1);
    filterTypeMenu.addItem("Bandpass", 2);
    filterTypeMenu.addItem("HighPass", 3);
    filterTypeMenu.setText("Filter Type:", dontSendNotification);
    addAndMakeVisible(filterTypeMenu);

    filterTypeMenuAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "FILTERTYPEMENU", filterTypeMenu);
    // filterOnOffButton.onClick() = [this] { audioProcessor.filterIsOn(); }; //Instead to add a Button::Listener and use the pure virtual buttonClicked() I use this labda function which return a function just when the button is clicked
    filterTypeMenu.onChange = [this]() {
        filterTypeInt = filterTypeMenu.getSelectedItemIndex();
        filterTypeEnum = static_cast<FilterType>(filterTypeInt + 1); // Cast it to your enum type
        DBG("FilterEnum is: " << filterTypeEnum);
        audioProcessor.setFilterType(filterTypeEnum);
        // DBG("Nuevo índice seleccionado: " << audioProcessor.filterType);
    };

    syncBpmMenu.addItem("Manual", 1);
    syncBpmMenu.addItem("1/2", 2);
    syncBpmMenu.addItem("1/4", 3);
    syncBpmMenu.addItem("1/3", 4);
    syncBpmMenu.addItem("3/4", 5);
    syncBpmMenu.setText("Sync Time", dontSendNotification);
    addAndMakeVisible(syncBpmMenu);

    syncBpmMenuAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "SYNCBpmMENU", syncBpmMenu);

    syncBpmMenu.onChange = [this]() {
        syncTimeIndexMenu = syncBpmMenu.getSelectedItemIndex();
        audioProcessor.setSyncTime(syncTimeIndexMenu);
        //DBG("El Index Selecionado es: " << syncTimeIndexMenu);
    };

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
    sldrFreqCutoff.setLookAndFeel(&filterLookAndFeel);
    addAndMakeVisible(sldrFreqCutoff);

    filterCutoffAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FILTERCUTOFF", sldrFreqCutoff);

    freqCutoffLabel.setText("Freq Cutoff", dontSendNotification);
    freqCutoffLabel.attachToComponent(&sldrFreqCutoff, false);
    freqCutoffLabel.setJustificationType(Justification::centredBottom);
    addAndMakeVisible(freqCutoffLabel);
}

CircularAudioBufferAudioProcessorEditor::~CircularAudioBufferAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void CircularAudioBufferAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    background = ImageCache::getFromMemory(BinaryData::brackgorund612x612_jpg, BinaryData::brackgorund612x612_jpgSize);
    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), RectanglePlacement::stretchToFit);

}

void CircularAudioBufferAudioProcessorEditor::resized()
{
    auto area = getBounds();
    sldrDelayTime.setBounds(70, 50, 265, 200);
    sldrDelayGain.setBounds(410, 50, 265, 200);
    filterOnOffButton.setBounds(700, 50, 50, 30);
    filterTypeMenu.setBounds(700, 90, 100, 30);
    syncBpmMenu.setBounds(300, 50, 100, 20);
    sldrFreqCutoff.setBounds(800, 55, 90, 90);
}

void CircularAudioBufferAudioProcessorEditor::updateSliderColour(float value)
{
    auto desiredColour = (value > 1.0f) ? Colours::red : Colours::transparentWhite;

    // Asegurarse de que la actualización ocurre en el hilo de mensaje
    juce::MessageManager::callAsync([this, desiredColour]()
        {
            delayGainLabel.setColour(juce::Label::backgroundColourId, desiredColour);
        });
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
