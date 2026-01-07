/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
GhostlineAudioProcessor::GhostlineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Get parameter pointers
    delayTimeParam = apvts.getRawParameterValue("DELAYTIME");
    feedbackParam = apvts.getRawParameterValue("FEEDBACK");
    wetLevelParam = apvts.getRawParameterValue("WET");
    dryLevelParam = apvts.getRawParameterValue("DRY");
    modulationRateParam = apvts.getRawParameterValue("MODRATE");
    modulationDepthParam = apvts.getRawParameterValue("MODDEPTH");
    
    // Initialize delay buffers with default size (will be resized in prepareToPlay)
    delayBuffer.resize(2);
    for (int ch = 0; ch < 2; ++ch)
    {
        delayBuffer[ch].resize(delayBufferSize, 0.0f);
        
        // Initialize smoothed delay time
        smoothedDelayTime[ch].reset(44100.0, 0.05); // 50ms ramp time
        smoothedDelayTime[ch].setCurrentAndTargetValue(cachedDelayTime);
    }
}

GhostlineAudioProcessor::~GhostlineAudioProcessor()
{
}

//==============================================================================
const juce::String GhostlineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GhostlineAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GhostlineAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GhostlineAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GhostlineAudioProcessor::getTailLengthSeconds() const
{
    return 2.0; // Tail for delay feedback
}

int GhostlineAudioProcessor::getNumPrograms()
{
    return 1;
}

int GhostlineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GhostlineAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GhostlineAudioProcessor::getProgramName (int index)
{
    return {};
}

void GhostlineAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GhostlineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    delayBufferSize = static_cast<int> (sampleRate * 1.0); // 1 second max delay
    
    // Resize and reinitialize delay buffers
    for (int ch = 0; ch < 2; ++ch)
    {
        delayBuffer[ch].resize(delayBufferSize, 0.0f);
        writePosition[ch] = 0;
        lfoPhase[ch] = 0.0f;
        
        // Initialize smoothed delay time with ramp time of 50ms
        smoothedDelayTime[ch].reset(sampleRate, 0.05);
        smoothedDelayTime[ch].setCurrentAndTargetValue(cachedDelayTime);
    }
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());
    
    wetGain.prepare (spec);
    dryGain.prepare (spec);
    
    updateParameters();
}

void GhostlineAudioProcessor::releaseResources()
{
    wetGain.reset();
    dryGain.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GhostlineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GhostlineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Safety check - ensure delay buffers are initialized
    if (delayBuffer.size() < 2 || delayBuffer[0].size() == 0 || delayBuffer[1].size() == 0)
        return;

    updateParameters();
    
    const int numSamples = buffer.getNumSamples();
    const float sampleRate = static_cast<float> (currentSampleRate);
    
    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels && channel < 2; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        const float feedback = cachedFeedback;
        const float wet = cachedWetLevel;
        const float dry = cachedDryLevel;
        const float modRate = cachedModulationRate;
        const float modDepth = cachedModulationDepth;
        
        const float lfoIncrement = (modRate * 10.0f) / sampleRate; // 0-10 Hz modulation
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get smoothed delay time (updates smoothly per sample)
            const float currentSmoothedDelay = smoothedDelayTime[channel].getNextValue();
            
            // Update LFO phase
            lfoPhase[channel] += lfoIncrement;
            if (lfoPhase[channel] >= 1.0f)
                lfoPhase[channel] -= 1.0f;
            
            // Calculate modulated delay time using smoothed delay time
            const float lfoValue = std::sin (lfoPhase[channel] * juce::MathConstants<float>::twoPi);
            const float modulatedDelay = currentSmoothedDelay + (lfoValue * modDepth * 0.01f); // Max 10ms modulation
            float delaySamples = modulatedDelay * sampleRate;
            
            // Clamp delay to buffer size to prevent out-of-bounds access
            delaySamples = juce::jlimit (1.0f, static_cast<float> (delayBufferSize - 1), delaySamples);
            
            // Calculate read position with fractional precision for smooth interpolation
            float readPos = static_cast<float>(writePosition[channel]) - delaySamples;
            if (readPos < 0.0f)
                readPos += static_cast<float>(delayBufferSize);
            
            // Wrap read position
            while (readPos >= static_cast<float>(delayBufferSize))
                readPos -= static_cast<float>(delayBufferSize);
            while (readPos < 0.0f)
                readPos += static_cast<float>(delayBufferSize);
            
            // Linear interpolation with proper bounds checking
            const int readPosInt = static_cast<int> (readPos);
            const float fraction = readPos - static_cast<float>(readPosInt);
            int readPosNext = readPosInt + 1;
            if (readPosNext >= delayBufferSize)
                readPosNext = 0;
            
            // Safety check for array bounds
            float delayedSample = 0.0f;
            if (readPosInt >= 0 && readPosInt < delayBufferSize && 
                readPosNext >= 0 && readPosNext < delayBufferSize)
            {
                delayedSample = delayBuffer[channel][readPosInt] * (1.0f - fraction) +
                              delayBuffer[channel][readPosNext] * fraction;
            }
            
            // Mix dry and wet
            const float input = channelData[sample];
            const float output = (input * dry) + (delayedSample * wet);
            
            // Write to delay buffer with feedback
            delayBuffer[channel][writePosition[channel]] = input + (delayedSample * feedback);
            writePosition[channel] = (writePosition[channel] + 1) % delayBufferSize;
            
            channelData[sample] = output;
        }
    }
}

//==============================================================================
bool GhostlineAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* GhostlineAudioProcessor::createEditor()
{
    return new GhostlineAudioProcessorEditor (*this);
}

//==============================================================================
void GhostlineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void GhostlineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
void GhostlineAudioProcessor::updateParameters()
{
    const float tolerance = 0.001f;
    
    // Safety check - ensure parameters are initialized
    if (delayTimeParam == nullptr || feedbackParam == nullptr || 
        wetLevelParam == nullptr || dryLevelParam == nullptr ||
        modulationRateParam == nullptr || modulationDepthParam == nullptr)
        return;
    
    float delayTime = delayTimeParam->load();
    float feedback = feedbackParam->load();
    float wet = wetLevelParam->load();
    float dry = dryLevelParam->load();
    float modRate = modulationRateParam->load();
    float modDepth = modulationDepthParam->load();
    
    if (std::abs (delayTime - cachedDelayTime) > tolerance)
    {
        cachedDelayTime = delayTime;
        
        // Update smoothed delay time target for smooth transitions
        for (int ch = 0; ch < 2; ++ch)
        {
            smoothedDelayTime[ch].setTargetValue(delayTime);
        }
    }
    
    if (std::abs (feedback - cachedFeedback) > tolerance)
    {
        cachedFeedback = feedback;
    }
    
    if (std::abs (wet - cachedWetLevel) > tolerance)
    {
        wetGain.setGainLinear (wet);
        cachedWetLevel = wet;
    }
    
    if (std::abs (dry - cachedDryLevel) > tolerance)
    {
        dryGain.setGainLinear (dry);
        cachedDryLevel = dry;
    }
    
    if (std::abs (modRate - cachedModulationRate) > tolerance)
    {
        cachedModulationRate = modRate;
    }
    
    if (std::abs (modDepth - cachedModulationDepth) > tolerance)
    {
        cachedModulationDepth = modDepth;
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GhostlineAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Delay Time: 0.01 to 1.0 seconds, default 0.3
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("DELAYTIME", 1), "Delay Time",
        juce::NormalisableRange<float> (0.01f, 1.0f, 0.01f, 0.3f),
        0.3f, "s"
    ));

    // Feedback: 0.0 to 0.95, default 0.3
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("FEEDBACK", 1), "Feedback",
        juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f),
        0.3f
    ));

    // Wet Level: 0.0 to 1.0, default 0.5
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("WET", 1), "Wet Level",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.5f
    ));

    // Dry Level: 0.0 to 1.0, default 0.5
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("DRY", 1), "Dry Level",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.5f
    ));

    // Modulation Rate: 0.0 to 1.0, default 0.5
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("MODRATE", 1), "Modulation Rate",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.5f
    ));

    // Modulation Depth: 0.0 to 1.0, default 0.0
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("MODDEPTH", 1), "Modulation Depth",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
        0.0f
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GhostlineAudioProcessor();
}

