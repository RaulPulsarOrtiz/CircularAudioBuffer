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

/** This function copies the (normal) buffer into the circular Delay Bufer.
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

/** This function is taking the ouput of the main buffer and copy it to the delayBuffer writePos
 * To be sure that we are not coming back to much on the time that we reach the edge "if (delayBufferSize > bufferSize + writePos)" 
*/
    void feedbackDelay(int channel, const int bufferSize, const int delayBufferSize, float* ouputDryBuffer, float delayGain);

/** Global variable to select the type of fixed delayTime based on the BPM from the Host*/
    int typeOfSyncTime = 0;

/**
* Set the type of filter choosen on the ComboBox "filterTypeMenu"
* @param The type of filter is set as an index of the "filterTypeMenu" in the Editor
* After choosing between LPF, BPF and HPF, the FILTERCUTOFF is set to be 20000 Hz, 1500 Hz or 500 Hz respectively 
*/
    void setFilterType(int newfilterType);
    int filterType = 0;
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
    int sampleRate{ 44100 };
    
    /** Delay Time in milisecond*/
    float delayTime = { 0.f }; // I declare this value as float because the SmoothValue ramp is double (and the valueTreeState does not accept doubles) and also if it were int I would have to round that ramp, causing possible jumps in the value of this variable.
    float delayGain{ 1.f };   

    /**Resets filter configuration*/
    void reset() override;

    /** Type of JUCE dsp filter declaration for post-feedback filtering */
    dsp::StateVariableTPTFilter<float> filter;  
    float filterFreqCutOff;
    bool filterState = false;  

    /** SmoothValue to smooth the changing of delayTime */
    LinearSmoothedValue<int> interpol;

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

    /** CrossFade variables
    * The crossfadeFactor is a variable that determines the current state of the crossfade. In this case, this value starts at 0 and gradually increases until it reaches 1. During the crossfade process, its value is incremented, allowing the signal to change from one read position in the buffer to another (from the old to the new) gradually and smoothly, avoiding artifacts or clicks.. Durante el proceso de crossfade, su valor va incrementando, lo que permite que la señal cambie de una posición de lectura en el buffer a otra(de la vieja a la nueva) de forma gradual y suave, evitando artefactos o clics.
 
    * When the crossfade Factor is 0, you are hearing only the signal read from the old position (oldReadPosition).
    * When the crossfadeFactor is 1, you are listening only to the signal read from the new position (newReadPosition).
    * During the process, the value goes from 0 to 1, progressively mixing both signals: the one from the old position decreases while the one from the new position increases.
    */
    bool crossfadeInProgress = false;   // To know if the crossfade is in progress
    float crossfadeFactor = 0.0f;       // Crossfade factor will go from 0 to 1                                      
    float crossfadeStep = { 0.09f };    // Step by which crossfadeFactor increases 0.001f defines the rate at which the crossfadeFactor increases with each iteration of audio processing
    int oldReadPosition = { 0 };        // Reading position before crossfade
    int newReadPosition = { 0 };        // New reading position after crossfade

    /** Filter applied to the delayTime value to smooth the value and achieve a pitch shift effect */
    dsp::IIR::Filter<double> interpolFilter;
    /** Filter to avoid noises and high pitch beeps getting into the feedback after changing the delayTime effect */
    dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> endFilter; // Declaration to use this type of filter in a stereo channel
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessor)
};
   