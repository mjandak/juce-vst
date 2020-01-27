/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class DelayAudioProcessor : public AudioProcessor
{
public:
	//AudioParameterInt* mDelayTimeParam;
	//AudioParameterFloat* mFeedbackParam;
	int mDelayTime{ 500 };
	AudioProcessorValueTreeState tree;

	//==============================================================================
	DelayAudioProcessor();
	~DelayAudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;

	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	//==============================================================================
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	void SetDelayTime(int ms);

private:
	//delay buffer
	AudioBuffer<float> mDelayBuffer;

	//current write position in delay buffer
	int mDelayBufferWritePosition{ 0 };

	//samples per second
	double mSampleRate{ 44100 };

	std::atomic<float*> m_DelayTimeParameter = nullptr;
	std::atomic<float*> m_FeedbackParameter = nullptr;

	void CopyToDelayBuffer(int channel, int bufferLength, int delayBufferLength, const float* bufferData, const float* delayBufferData);
	void AddFromDelayBuffer(int channel, AudioBuffer<float>& buffer, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
