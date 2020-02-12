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

private:
	//delay buffer
	AudioBuffer<float> m_DelayBuffer;
	AudioBuffer<float> m_filterBuffer;
	juce::dsp::AudioBlock<float>* m_filterBlock = nullptr;
	dsp::LadderFilter<float> m_filter1;
	dsp::LadderFilter<float> m_filter2;

	//current write position in delay buffer
	int m_delayBufferWritePosition{ 0 };
	int m_delayedBlockPosition{ 0 };
	//samples per second
	double m_sampleRate{ 44100.0 };

	std::atomic<float>* m_DelayTimeParameter = nullptr;
	std::atomic<float>* m_FeedbackParameter = nullptr;
	std::atomic<float>* m_DryWetParameter = nullptr;
	std::atomic<float>* m_CutOff = nullptr;

	void DelayBufferInput(int channel, AudioBuffer<float>& buffer);
	void DelayBufferFeedback(int channel, AudioBuffer<float>& buffer);
	void DWMix(int channel, AudioBuffer<float>& buffer);

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessor)
};
