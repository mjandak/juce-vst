/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessor::DelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	), tree(*this, nullptr, "PARAMETERS", {
		std::make_unique<AudioParameterInt>("delaytime", "Delay time", 0, 2000, 250),
		std::make_unique<AudioParameterFloat>("feedback", "Feedback", 0.0, 1.0, 0.6),
		std::make_unique<AudioParameterFloat>("drywet", "DryWet", 0.0, 1.0, 0.5)
		}
	)
#endif
{
	m_DelayTimeParameter = tree.getRawParameterValue("delaytime");
	m_FeedbackParameter = tree.getRawParameterValue("feedback");
	m_DryWetParameter = tree.getRawParameterValue("drywet");
}

DelayAudioProcessor::~DelayAudioProcessor()
{
}

//==============================================================================
const String DelayAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool DelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool DelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool DelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double DelayAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int DelayAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int DelayAudioProcessor::getCurrentProgram()
{
	return 0;
}

void DelayAudioProcessor::setCurrentProgram(int index)
{
}

const String DelayAudioProcessor::getProgramName(int index)
{
	return {};
}

void DelayAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void DelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	//two seconds of delay time plus little bit of extra room
	const int delayBufferSize = 2 * (sampleRate + samplesPerBlock);

	mDelayBuffer.setSize(getTotalNumInputChannels(), delayBufferSize);
	mDelayBuffer.clear();
}

void DelayAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void DelayAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	const int bufferLength = buffer.getNumSamples();
	const int delayBufferLength = mDelayBuffer.getNumSamples();

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		//auto* channelData = buffer.getWritePointer (channel);
		auto bufferData = buffer.getReadPointer(channel);
		auto delayBufferData = mDelayBuffer.getReadPointer(channel);
		DelayBufferInput(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
		Feedback(channel, buffer, bufferLength, delayBufferLength, bufferData, delayBufferData);
		DWMix(channel, buffer, bufferLength, delayBufferData, delayBufferLength);
	}

	//shift delay buffer write position by main buffer's length
	m_delayBufferWritePosition += bufferLength;
	//if write position exceeds delay buffer length, wrap around
	m_delayBufferWritePosition %= delayBufferLength;
}

//Copy bufferData to delayBufferData at m_delayBufferWritePosition
void DelayAudioProcessor::DelayBufferInput(int channel, int bufferLength, int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
	//number of empty samples left in delay buffer:
	//(last_sample_idx - first_empty_sample_idx) + 1 = last_sample_idx + 1 - first_empty_sample_idx = delay_buffer_length - first_empty_sample_idx
	const int emptySamplesLeft = delayBufferLength - m_delayBufferWritePosition;

	//is there in delay buff enough space to fit main buff?
	if (emptySamplesLeft >= bufferLength)
	{
		//there is
		mDelayBuffer.copyFrom(channel, m_delayBufferWritePosition, bufferData, bufferLength);
	}
	else
	{
		//there is not
		mDelayBuffer.copyFrom(channel, m_delayBufferWritePosition, bufferData, emptySamplesLeft);
		mDelayBuffer.copyFrom(channel, 0, bufferData + emptySamplesLeft, bufferLength - emptySamplesLeft);
	}
}

//Add delay buffer's output to it's input
void DelayAudioProcessor::Feedback(int channel, AudioBuffer<float>& buffer, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
	//delay time in ms
	int delayTime = *m_DelayTimeParameter;
	float feedback = *m_FeedbackParameter;

	//sample rate = samples per second
	const int samplesPerDelayTime = std::lround(getSampleRate() * (delayTime / 1000.0));
	//go back in time
	m_delayedBlockPosition = m_delayBufferWritePosition - samplesPerDelayTime; // -1 ???
	if (m_delayedBlockPosition < 0)
	{
		m_delayedBlockPosition = delayBufferLength - 1 + m_delayedBlockPosition;
	}

	for (int i = 0; i < bufferLength; i++)
	{
		auto delayeSamplePosition = (m_delayedBlockPosition + i) % delayBufferLength;
		auto delayedSample = mDelayBuffer.getSample(channel, delayeSamplePosition);
		auto currentSamplePosition = (m_delayBufferWritePosition + i) % delayBufferLength;
		auto currentSample = mDelayBuffer.getSample(channel, currentSamplePosition);
		mDelayBuffer.setSample(channel, currentSamplePosition, (feedback * delayedSample) + currentSample);
	}
}

void DelayAudioProcessor::DWMix(int channel, AudioBuffer<float>& buffer, int bufferLength, const float* delayBufferData, int delayBufferLength)
{
	float dryWet = *m_DryWetParameter;
	buffer.applyGain(channel, 0, bufferLength, 1.0f - dryWet);
	const int numOfRemainingSamplesInDelayBuff = delayBufferLength - m_delayedBlockPosition;
	if (numOfRemainingSamplesInDelayBuff >= bufferLength)
	{
		buffer.addFrom(channel, 0, delayBufferData + m_delayedBlockPosition, bufferLength, dryWet);
	}
	else
	{
		buffer.addFrom(channel, 0, delayBufferData + m_delayedBlockPosition, numOfRemainingSamplesInDelayBuff, dryWet);
		buffer.addFrom(channel, 0, delayBufferData, bufferLength - numOfRemainingSamplesInDelayBuff, dryWet);
	}

}

//==============================================================================
bool DelayAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DelayAudioProcessor::createEditor()
{
	return new DelayAudioProcessorEditor(*this);
}

//==============================================================================
void DelayAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	//MemoryOutputStream(destData, true).writeInt(*mDelayTimeParam);
}

void DelayAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	//*mDelayTimeParam = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readInt();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new DelayAudioProcessor();
}
