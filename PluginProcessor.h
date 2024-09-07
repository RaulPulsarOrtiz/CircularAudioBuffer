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
  
    /**  Gets the index selected in the syncBpmMenu comboBox to select a sync fraction of a bar and set the Delay Time to be that value
    *@param It is the selected index in the comboBox
    */
    void setSyncTime(int newSyncTimeIndex);

/** This function copies the normal buffer (given by JUCE????) into the circular Delay Bufer.
* Firstly it checks if the main buffer copies to delay buffer without needing to wrap. "if (delayBufferSize > bufferSize + writePos)"
* If yes.
* It copies main buffer contents to delay buffer.
* If not
* Determine how much space is left at the end of the delay buffer "auto numSamplesToEnd = delayBufferSize - writePos;"
*   Copy that amount of contents to the end...
*   Calculate how much content is remaining to copy from the normal buffer " auto numSampleAtStart = bufferSize - numSamplesToEnd;"
*   Copy remaining amount to beginning of delay buffer 
*/
    void fillDelayBuffer(int channel, const int bufferSize, const int delayBufferSize, const float* bufferData, const float* delayBufferData);
   
/** Content in circular Delay Buffer is copied again into normal Buffer with a delay applied
* "mSampleRate * delayTime/1000" -> this is converting the seconds of delay (500ms) in samples. static_cast<int> is = than (int)(something) to be sure that everything that is there is going to be casted as an int
* To be sure that we are not coming back to much on the time that we reach the edge "if (delayBufferSize > bufferSize + readPosition)" 
*/
    void getFromDelayBuffer(AudioBuffer<float> buffer, int channel, const int bufferSize, const int delayBufferSize, const float* bufferData, const float* delayBufferData);
   
/** This function is taking the ouput of the main buffer and copy it to the delayBuffer
        To be sure that we are not coming back to much on the time that we reach the edge "if (delayBufferSize > bufferSize + writePos)" 
*/
    void feedbackDelay(int channel, const int bufferSize, const int delayBufferSize, float* ouputDryBuffer, float delayGain);

    int filterType = 0;

    int typeOfSyncTime = 0;
/**
* Set the type of filter choosen on the ComboBox "filterTypeMenu"
* @param The type of filter is set as an index of the "filterTypeMenu" in the Editor
* After choosing between LPF, BPF and HPF, the FILTERCUTOFF is set to be 20000 Hz, 1500 Hz or 500 Hz respectively 
*/
    void setFilterType(int newfilterType);

/** Declaration of the AudioProcessorValueTreeState object. This object holds all the parameters */
    AudioProcessorValueTreeState apvts;

/** Creates a list of parameters thar are linked with the sliders on a ValueTreeState
* @return the list of parameters 
*/
    AudioProcessorValueTreeState::ParameterLayout createParameters();

/** Method to get the parameters from the ValueTreeState
* @return and object with of the ValueTreeState to access their parameters 
*/
    AudioProcessorValueTreeState& getValueTreeState();

private:
    /** Circular Buffer where I will copy my buffer to applies a delay to it and read again from there */
    AudioBuffer<float> delayBuffer;
    
    /** Writing position to the Circular Delay Buffer */
    int writePos { 0 };

    /** Used to multiply for the Delay Time in milisecond and get the number of samples that de signal is delayed */
    int mSampleRate{ 44100 };

    /** Writing position to the Circular Delay Buffer */
    int readPosition[48509];

    /** Delay Time in milisecond*/
    int delayTime { 0 };

    float delayGain{ 1.f };

    //float fDelayBufferFiltered{ 0 };

    std::atomic<int> freqCutoff = 20000;

    /**Resets filter configuration*/
    void reset() override;

    /** Type of JUCE dsp filter declaration */
    dsp::StateVariableTPTFilter<float> filter;
    
    float filterFreqCutOff;
    bool filterState = false;
        
    //CatmullRomInterpolator interpol;
    LinearSmoothedValue<float> interpol;

    /** Gets and object of the audioPlayHead to get the BPM info from the Host
    * It calculates different fractions of a bar in milisecond
    */
    AudioPlayHead* audioPlayHead;
    AudioPlayHead::CurrentPositionInfo hostInfo;
    int barDuration{ 0 };
    int halfBar{ 0 };
    int quarterBar{ 0 };
    int thirdBar{ 0 };
    int threeQuarterBar{ 0 };
    int syncValue{ 0 };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessor)
};
