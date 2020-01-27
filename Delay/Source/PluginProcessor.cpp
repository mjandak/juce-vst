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
		std::make_unique<AudioParameterFloat>("feedback", "Feedback", 0.0, 1.0, 0.6) })
#endif
{
	//addParameter(mDelayTimeParam = new AudioParameterInt(
	//	"delayTime", // parameter ID
	//	"DelayTime", // parameter name
	//	0,   // minimum value
	//	2000,   // maximum value
	//	500)); // default value

	//addParameter(mFeedbackParam = new AudioParameterFloat(
	//	"feedback", // parameter ID
	//	"Feedback", // parameter name
	//	0.0,   // minimum value
	//	1.0,   // maximum value
	//	0.7)); // default value

	m_DelayTimeParameter = tree.getRawParameterValue("delaytime");
	m_FeedbackParameter = tree.getRawParameterValue("feedback");
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
	mSampleRate = sampleRate;

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
		AddFromDelayBuffer(channel, buffer, bufferLength, delayBufferLength, bufferData, delayBufferData);
		CopyToDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
	}

	//shift delay buffer write position by main buffer's length
	mDelayBufferWritePosition += bufferLength;
	//if write position exceeds delay buffer length, wrap around
	mDelayBufferWritePosition %= delayBufferLength;
}

void DelayAudioProcessor::CopyToDelayBuffer(int channel, int bufferLength, int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
	//number of empty samples left in delay buffer:
	//(last_sample_idx - first_empty_sample_idx) + 1 = last_sample_idx + 1 - first_empty_sample_idx = delay_buffer_length - first_empty_sample_idx
	const int emptySamplesLeft = delayBufferLength - mDelayBufferWritePosition;

	//is there in delay buff enough space to fit main buff?
	if (emptySamplesLeft >= bufferLength)
		//if (delayBufferLength > mDelayBufferWritePosition + bufferLength)
	{
		//there is
		//copy bufferData to delayBufferData at mDelayBufferWritePosition
		mDelayBuffer.copyFrom(channel, mDelayBufferWritePosition, bufferData, bufferLength);
	}
	else
	{
		//there is not
		mDelayBuffer.copyFrom(channel, mDelayBufferWritePosition, bufferData, emptySamplesLeft);
		mDelayBuffer.copyFrom(channel, 0, bufferData + emptySamplesLeft, bufferLength - emptySamplesLeft);
	}
}

void DelayAudioProcessor::AddFromDelayBuffer(int channel, AudioBuffer<float>& buffer, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
	//delay time in ms
	int delayTime = *m_DelayTimeParameter;
	float feedback = *m_FeedbackParameter;

	//const int readPosition = static_cast<int>(delayBufferLength + mDelayBufferWritePosition - (mSampleRate * delayTime / 1000)) % delayBufferLength;

	//if (delayBufferLength > bufferLength + readPosition)
	//{
	//	buffer.addFrom(channel, 0, delayBufferData + readPosition, bufferLength);
	//}

	int samplesPerDelayTime = std::lround(mSampleRate * (delayTime / 1000.0f));
	//go back in time
	int delayBufferReadPosition = mDelayBufferWritePosition - samplesPerDelayTime; // -1 ???
	if (delayBufferReadPosition < 0)
	{
		delayBufferReadPosition = delayBufferLength - 1 + delayBufferReadPosition;
	}

	//now mix main buffer with data from delay buffer starting at delayBufferReadPosition
	//is there enough samples in delay buff to fill main buff (from delayBufferReadPosition to the end of delay buff)?
	//number of remaining samples in delay buff:
	//(delayBufferLength - 1 - delayBufferReadPosition) + 1 = delayBufferLength - delayBufferReadPosition
	int numOfRemainingSamplesInDelayBuff = delayBufferLength - delayBufferReadPosition;
	if (numOfRemainingSamplesInDelayBuff >= bufferLength)
	{
		//there is
		buffer.addFrom(channel, 0, delayBufferData + delayBufferReadPosition, bufferLength, feedback);
		//buffer.copyFrom(channel, 0, delayBufferData + delayBufferReadPosition, bufferLength);
	}
	else
	{
		//there is not
		buffer.addFrom(channel, 0, delayBufferData + delayBufferReadPosition, numOfRemainingSamplesInDelayBuff, feedback);
		buffer.addFrom(channel, 0 + numOfRemainingSamplesInDelayBuff, delayBufferData + 0, bufferLength - (numOfRemainingSamplesInDelayBuff), feedback);
		//buffer.copyFrom(channel, 0, delayBufferData + delayBufferReadPosition, numOfRemainingSamplesInDelayBuff);
		//buffer.copyFrom(channel, 0 + numOfRemainingSamplesInDelayBuff, delayBufferData + 0, bufferLength - (numOfRemainingSamplesInDelayBuff));
	}
}

void DelayAudioProcessor::SetDelayTime(int ms)
{
	//mDelayTimeParam->setValueNotifyingHost(ms);
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
