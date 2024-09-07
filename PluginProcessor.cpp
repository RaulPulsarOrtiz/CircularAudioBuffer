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

void CircularAudioBufferAudioProcessor::setFilterType(int newfilterType) 
{
    filterType = newfilterType;
  
    if (filterType == 1) // FilterType::LowPass)
    {
        filter.setType(dsp::StateVariableTPTFilterType::lowpass);
        getValueTreeState().getParameterAsValue("FILTERCUTOFF") = 20000.0f; // Cambia el valor del parámetro 'filterCutoff' a 500
        DBG("Filter is set to: " << filterType);
    }
    else if (filterType == 2) //FilterType::BandPass)
    {
        filter.setType(dsp::StateVariableTPTFilterType::bandpass);
        getValueTreeState().getParameterAsValue("FILTERCUTOFF") = 2000.0f;
    }
    else if (filterType == 3) ///FilterType::HighPass)
    {
         filter.setType(dsp::StateVariableTPTFilterType::highpass);
         getValueTreeState().getParameterAsValue("FILTERCUTOFF") = 500.0f; // Cambia el valor del parámetro 'filterCutoff' a 500 //getParameterAsValue ->Returns a Value object that can be used to control a particular parameter.
         DBG("Filter is set to: " << filterType);
    }
}

void CircularAudioBufferAudioProcessor::reset()
{
    filter.reset();
}

void CircularAudioBufferAudioProcessor::setSyncTime(int newSyncTimeIndex)
{
    auto* delayTimeParameter = getValueTreeState().getParameter("DELAYTIME");
    typeOfSyncTime = newSyncTimeIndex;

    if (typeOfSyncTime == 0) {
       // delayTime = delayTime; //What happens if I change to sync and to manual again?
        getValueTreeState().getParameter("DELAYTIME")->setValueNotifyingHost(delayTime); //MMM siempre va a tener el anterior valor
    }
    else if (typeOfSyncTime == 1) {
        float normalisedValue = delayTimeParameter->convertTo0to1(halfBar);  // Normalización de halfBar
        getValueTreeState().getParameter("DELAYTIME")->setValueNotifyingHost(normalisedValue);
    }
    else if (typeOfSyncTime == 2) {
        float normalisedValue = delayTimeParameter->convertTo0to1(quarterBar); // Normalización de quarterBar
        getValueTreeState().getParameter("DELAYTIME")->setValueNotifyingHost(normalisedValue);
     }
    else if (typeOfSyncTime == 3) {
       float normalisedValue = delayTimeParameter->convertTo0to1(thirdBar); // Normalización de thirdBar
       getValueTreeState().getParameter("DELAYTIME")->setValueNotifyingHost(normalisedValue);  //With 120bpm this is more than 1000 ms (1500)
    }
    else if (typeOfSyncTime == 4) {
        float normalisedValue = delayTimeParameter->convertTo0to1(threeQuarterBar); // Normalización de threeQuarterBar
        getValueTreeState().getParameter("DELAYTIME")->setValueNotifyingHost(normalisedValue);
    }
    //DBG("Menu typo is: " << typeOfSyncTime);
    //DBG("delayTime is: " << delayTime);
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
    //int barDuration = (4.0 / (hostInfo.bpm / 60.0)); //beat Signature 4/4
    delayTime = *apvts.getRawParameterValue("DELAYTIME");
    interpol.setTargetValue(delayTime);  //Smoothing parameter to avoid zero cross issues with SmoothingValue. -> Change this for cumtull ROM
    delayTime = interpol.getNextValue();


    // for (int sample = 0; sample < delayBuffer.getNumSamples(); ++sample)
    // {

    const int readPosition = (delayBufferSize + writePos - (mSampleRate * delayTime / 1000)) % delayBufferSize;  //(mSampleRate * delayTime/1000) -> this is converting the seconds of delay (500ms) in samples. static_cast<int> is = than (int)(something) to be sure that everything that is there is going to be casted as an int
  //  DBG("DelayTime in the action is being: " << delayTime);
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
   
    /** @param barDuration is the duration of a bar. First in seconds and after is updated to miliseconds */
    audioPlayHead = this->getPlayHead();
    if (audioPlayHead != nullptr)
    {
        audioPlayHead->getCurrentPosition(hostInfo);

        barDuration = 4 * (60 / hostInfo.bpm) * 1000;  //  (4.0 / (hostInfo.bpm * 60.0)); //beat Signature 4/4 //now miliseconds

        halfBar = barDuration * 0.5;
        quarterBar = barDuration * 0.25;
        thirdBar = barDuration * 0.33;
        threeQuarterBar = barDuration * 0.75;
    }

    

    // Verificar si el playHead está disponible
 //   if (auto* playHead = getPlayHead())
 //   {
 //       // Obtener la información de la posición actual
 //       if (playHead->getCurrentPosition(hostInfo))
 //       {
 //           // Extraer el BPM (tempo) del host
 //           double bpm = hostInfo.bpm;
 // 
 //           // Imprimir o usar el valor del BPM
 //           DBG("El tempo actual es: " << bpm << " BPM");
 //        }
 //   }

   // 
   // 
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

AudioProcessorValueTreeState::ParameterLayout CircularAudioBufferAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<AudioParameterFloat>("DELAYTIME", "DelayTime", 0.f, 1000.f, 0.f));
    params.push_back(std::make_unique<AudioParameterFloat>("DELAYFEEDBACK", "DelayFeedback", 0.f, 1.1f, 0.5f));
    params.push_back(std::make_unique<AudioParameterChoice>("FILTERTYPEMENU", "FilterTypeMenu", juce::StringArray{ "Lowpass", "Bandpass", "HighPass" }, 0));
    params.push_back(std::make_unique<AudioParameterBool>("FILTERONOFF", "filterOnOff", false)); 
    params.push_back(std::make_unique<AudioParameterFloat>("FILTERCUTOFF", "FilterCutoff", NormalisableRange<float>(500.0f, 20000.0f, 0.0f, 0.3f), 20000.0f)); // 0.3f controla la curva logarítmica. With the normalisable range the curve has much more detail on the low freq. Similar tu log curve
    //params.push_back(std::make_unique<AudioParameterFloat>("FILTERCUTOFF", "FilterCutoff", 500.f, 20000.f, 20000.f)); //Before
    params.push_back(std::make_unique<AudioParameterChoice>("SYNCBpmMENU", "syncBpmMenu", juce::StringArray{ "Manual", "1/2", "1/4", "1/3", "3/4",  }, 0));
    return { params.begin(), params.end() };                                                                
}                                                                                                            


AudioProcessorValueTreeState& CircularAudioBufferAudioProcessor::getValueTreeState() { return apvts; }

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
