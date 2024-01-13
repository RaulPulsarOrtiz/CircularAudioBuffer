/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class CircularAudioBufferAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CircularAudioBufferAudioProcessor();
    ~CircularAudioBufferAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setDelayTime(int delayTime);
    void setDelayGain(float delayGain);
    void fillDelayBuffer(int channel, const int bufferSize, const int delayBufferSize, const float* bufferData, const float* delayBufferData);
    void getFromDelayBuffer(AudioBuffer<float> buffer, int channel, const int bufferSize, const int delayBufferSize, const float* bufferData, const float* delayBufferData);
    void feedbackDelay(int channel, const int bufferSize, const int delayBufferSize, float* ouputDryBuffer, float delayGain);
private:
    AudioBuffer<float> delayBuffer;
    int writePos { 0 };
    int mSampleRate{ 44100 };

    int delayTime { 0 }; // miliseconds
    float delayGain{ 1.f };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessor)
};
