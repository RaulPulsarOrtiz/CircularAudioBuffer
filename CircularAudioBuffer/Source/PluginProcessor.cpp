/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularAudioBufferAudioProcessor::CircularAudioBufferAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

CircularAudioBufferAudioProcessor::~CircularAudioBufferAudioProcessor()
{
}

//==============================================================================
const juce::String CircularAudioBufferAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CircularAudioBufferAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CircularAudioBufferAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CircularAudioBufferAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CircularAudioBufferAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CircularAudioBufferAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CircularAudioBufferAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CircularAudioBufferAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CircularAudioBufferAudioProcessor::getProgramName (int index)
{
    return {};
}

void CircularAudioBufferAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CircularAudioBufferAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    auto delayBufferSize = 1.1 * sampleRate; // To give a bit of extra room. That's how Audio Programmer does it.
    mSampleRate = sampleRate;
    delayBuffer.setSize(getTotalNumInputChannels(), (int)delayBufferSize); //To cast buffer size that are double with the type of argument in the definition of the function


    dsp::ProcessSpec spec; //DSP algorithm needs this info to work
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    filter.prepare(spec);
    filter.setResonance(1 / sqrt(2)); //????
    reset(); //To avoid junk value from the previous time it was used
  //  filter.setType(dsp::StateVariableTPTFilterType::lowpass);
   // filter.setCutoffFrequency(500.f);

    interpol.reset(sampleRate, 0.0005);
}

void CircularAudioBufferAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CircularAudioBufferAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

//void CircularAudioBufferAudioProcessor::setDelayTime(int newDelayTime)
//{
//    delayTime = newDelayTime;
//    DBG("DelayTime is: " << newDelayTime);
//}

//void CircularAudioBufferAudioProcessor::setDelayGain(float newdelayGain)
//{
//    delayGain = newdelayGain;
//}

void CircularAudioBufferAudioProcessor::setFilterType(FilterType newFiltertype)
{
    filterType = newFiltertype;

    if (filterType == FilterType::LowPass)
    {
        filter.setType(dsp::StateVariableTPTFilterType::lowpass);
    }
    else if (filterType == FilterType::BandPass)
    {
        filter.setType(dsp::StateVariableTPTFilterType::bandpass);
    }
    else if (filterType == FilterType::HighPass)
    {
         filter.setType(dsp::StateVariableTPTFilterType::highpass);
    }
}

//void CircularAudioBufferAudioProcessor::setFreqCutoff(float newFreqCutoff)
//{
//    filterFreqCutOff = newFreqCutoff;
//}

void CircularAudioBufferAudioProcessor::reset()
{
    filter.reset();
}

void CircularAudioBufferAudioProcessor::fillDelayBuffer(int channel, const int bufferSize, const int delayBufferSize, const float* bufferData, const float* delayBufferData)
{
    const float gain = 0.7f;
    //Check to see if main buffer copies to delay buffer without needing to wrap
        //if yes
        //copy main buffer contents to delay buffer
    if (delayBufferSize > bufferSize + writePos)
    {

        delayBuffer.copyFromWithRamp(channel, writePos, bufferData, bufferSize, gain, gain);
    }

    //if not     
    else
    {
        //Determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - writePos;
        //Copy that amount of contents to the end...
        delayBuffer.copyFromWithRamp(channel, writePos, bufferData, numSamplesToEnd, gain, gain);

        //Calculate how much content is remaining to copy from the normal buffer
        auto numSampleAtStart = bufferSize - numSamplesToEnd;
        //Copy remaining amount to beginning of delay buffer
        delayBuffer.copyFromWithRamp(channel, 0, bufferData, numSampleAtStart, gain, gain);

    }
}

void CircularAudioBufferAudioProcessor::getFromDelayBuffer(AudioBuffer<float> buffer, int channel, const int bufferSize, const int delayBufferSize, const float* bufferData, const float* delayBufferData)
{
    delayTime = *apvts.getRawParameterValue("DELAYTIME");
    interpol.setTargetValue(delayTime);  //Smoothing parameter to avoid zero cross issues with SmoothingValue. -> Change this for cumtull ROM
    delayTime = interpol.getNextValue();
    DBG(delayTime);

    const int readPosition = static_cast<int>(delayBufferSize + writePos - (mSampleRate * delayTime / 1000)) % delayBufferSize;  //(mSampleRate * delayTime/1000) -> this is converting the seconds of delay (500ms) in samples. static_cast<int> is = than (int)(something) to be sure that everything that is there is going to be casted as an int
    
    if (delayBufferSize > bufferSize + readPosition) //To be sure that we are not coming back to much on the time that we reach the edge
    {
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferSize);
    }
    else
    {
        const int bufferRemaining = delayBufferSize - readPosition;
        buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.addFrom(channel, bufferRemaining, delayBufferData, bufferSize - bufferRemaining);
    }
}

void CircularAudioBufferAudioProcessor::feedbackDelay(int channel, const int bufferSize, const int delayBufferSize, float* ouputDryBuffer, float delayGain) //We are taking the ouput of the main buffer and copy it to the delayBuffer
{
    delayGain = *apvts.getRawParameterValue("DELAYFEEDBACK");

    if (delayBufferSize > bufferSize + writePos) //To be sure that we are not coming back to much on the time that we reach the edge
    {
        delayBuffer.addFrom(channel, writePos, ouputDryBuffer, bufferSize, delayGain);
    }

    else
    {
        const int bufferRemaining = delayBufferSize - writePos;
        delayBuffer.addFrom(channel, bufferRemaining, ouputDryBuffer, bufferRemaining, delayGain);
        delayBuffer.addFrom(channel, 0, ouputDryBuffer, bufferSize - bufferRemaining, delayGain);
    }
}

void CircularAudioBufferAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
 
    filterFreqCutOff = *apvts.getRawParameterValue("FILTERCUTOFF");
  
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) //Iterate for each channel of audio
    {
        auto* channelData = buffer.getWritePointer(channel);                   //	Returns a writeable pointer to one of the buffer's channels.
        auto* delayChannelData = delayBuffer.getWritePointer(channel);         //	Returns a writeable pointer to one of the delay buffer's channels.      

        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel);
        float* ouputDryBuffer = buffer.getWritePointer(channel);

        fillDelayBuffer(channel, bufferSize, delayBufferSize, bufferData, delayChannelData);
        getFromDelayBuffer(buffer, channel, bufferSize, delayBufferSize, bufferData, delayChannelData);
        // interpol.process(1.0, delayBufferData, delayBufferData, 2.0);
        feedbackDelay(channel, bufferSize, delayBufferSize, ouputDryBuffer, delayGain);
        
    }

    filter.setCutoffFrequency(filterFreqCutOff);
    filterState = *apvts.getRawParameterValue("FILTERONOFF");

    if (filterState == true)
    {
        /** Filter applied to delayBuffer before getFromDelayBuffer function, filtering per Block: */
        auto audioBlock = juce::dsp::AudioBlock<float>(buffer); //already do both channels for the dsp process
        auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);
        filter.process(context);
    }

    else if (filterState == false)
    {
        buffer = buffer; //??? what can I write here?
    }
    //Get channelData again or buffer and multiply for something less than 1 to avoid that rise and colapse of the L & R channels?



      writePos += bufferSize;  //To itinerate one position each time that the content has been copied
      writePos %= delayBufferSize; //This ensure that writePos is going to be between 0 and bufferSize

    //DBG("bufferDelaySize: " << delayBufferSize);
    //DBG("bufferSize: " << bufferSize);
    //DBG("writePos: " << writePos);
    
 
}


//void CircularAudioBufferAudioProcessor::filterIsOn(bool newFilterState)
//{
////    filterState = newFilterState;
//}

AudioProcessorValueTreeState::ParameterLayout CircularAudioBufferAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<AudioParameterFloat>("DELAYTIME", "DelayTime", 0.f, 1000.f, 0.f));
    params.push_back(std::make_unique<AudioParameterFloat>("DELAYFEEDBACK", "DelayFeedback", 0.f, 1.1f, 0.5f));
    params.push_back(std::make_unique<AudioParameterChoice>("FILTERTYPEMENU", "FilterTypeMenu", juce::StringArray{ "Lowpass", "HighPass" }, 0));
    params.push_back(std::make_unique<AudioParameterBool>("FILTERONOFF", "filterOnOff", false)); 
    params.push_back(std::make_unique<AudioParameterFloat>("FILTERCUTOFF", "FilterCutoff", 500.f, 20000.f, 20000.f));

    return { params.begin(), params.end() };
}

//==============================================================================
bool CircularAudioBufferAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CircularAudioBufferAudioProcessor::createEditor()
{
    return new CircularAudioBufferAudioProcessorEditor (*this);
}

//==============================================================================
void CircularAudioBufferAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CircularAudioBufferAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CircularAudioBufferAudioProcessor();
}
