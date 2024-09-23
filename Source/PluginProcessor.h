/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include <vld.h>

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
    int sampleRate{ 44100 };

    /** Writing position to the Circular Delay Buffer */
   // int readPosition[48509];
    
    /** Delay Time in milisecond*/
    float delayTime = { 0.f }; //Declaro este valor como float porque la rampa del SmoothValue es double (y el valueTreeState no acepta doubles) y ademas si fuese int tendria que redondear esa rampa, causando posibles saltos en el valor del esta variable.

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

    // Añadir estas variables para el control de crossfade
    bool crossfadeInProgress = false;   // Para saber si el crossfade está en progreso
    float crossfadeFactor = 0.0f;       // Factor de crossfade que irá de 0 a 1 
                                          //El crossfadeFactor es una variable que determina el estado actual del crossfade o fundido cruzado.En tu caso, este valor empieza en 0 y gradualmente aumenta hasta llegar a 1. Durante el proceso de crossfade, su valor va incrementando, lo que permite que la señal cambie de una posición de lectura en el buffer a otra(de la vieja a la nueva) de forma gradual y suave, evitando artefactos o clics.
                                          //
                                          //    Cuando el crossfad eFactor es 0, estás escuchando solo la señal leída desde la posición antigua(oldReadPosition).
                                          //    Cuando el crossfadeFactor es 1, estás escuchando solo la señal leída desde la nueva posición(newReadPosition).
                                          //    Durante el proceso, el valor va desde 0 hasta 1, mezclando progresivamente ambas señales : la de la posición antigua decrece mientras la de la posición nueva aumenta.
    
    float crossfadeStep = { 0.09f }; //0.01; //0.09f;       // Paso por el cual incrementa crossfadeFactor 0.001f  define la velocidad a la que el crossfadeFactor aumenta en cada iteración del procesamiento de audio
    int oldReadPosition = { 0 };            // Posición de lectura anterior para crossfade
    int newReadPosition = { 0 };            // Posición de lectura nueva para crossfade
    dsp::IIR::Filter<double> interpolFilter;
  //  dsp::IIR::Filter<double> endFilter;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircularAudioBufferAudioProcessor)
};
   