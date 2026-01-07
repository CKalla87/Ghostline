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
class GhostlineAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    GhostlineAudioProcessor();
    ~GhostlineAudioProcessor() override;

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

    //==============================================================================
    // Parameter management
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    // Delay parameters
    std::atomic<float>* delayTimeParam = nullptr;
    std::atomic<float>* feedbackParam = nullptr;
    std::atomic<float>* wetLevelParam = nullptr;
    std::atomic<float>* dryLevelParam = nullptr;
    std::atomic<float>* modulationRateParam = nullptr;
    std::atomic<float>* modulationDepthParam = nullptr;

private:
    //==============================================================================
    // DSP processing - Space Echo style delay with modulation
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::dsp::Gain<float> wetGain;
    juce::dsp::Gain<float> dryGain;
    
    // Dynamic delay buffers - allocated based on sample rate
    std::vector<std::vector<float>> delayBuffer;
    int delayBufferSize = 44100;
    int writePosition[2] = {0, 0};
    
    float lfoPhase[2] = {0.0f, 0.0f};
    
    void updateParameters();
    
    double currentSampleRate = 44100.0;

    // Cached parameter values
    float cachedDelayTime = 0.3f;
    float cachedFeedback = 0.3f;
    float cachedWetLevel = 0.5f;
    float cachedDryLevel = 0.5f;
    float cachedModulationRate = 0.5f;
    float cachedModulationDepth = 0.0f;
    
    // Smoothed delay time to prevent clicks when changing delay time
    juce::SmoothedValue<float> smoothedDelayTime[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GhostlineAudioProcessor)
};

