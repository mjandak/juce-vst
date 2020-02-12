/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <fstream>

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
		std::make_unique<AudioParameterFloat>("drywet", "DryWet", 0.0, 1.0, 0.5),
		std::make_unique<AudioParameterFloat>("cutoff", "CutOff", NormalisableRange<float>(20.0, 20000.0, 0.01, 0.21), 4000.0)
		}
	)
#endif
{
	m_DelayTimeParameter = tree.getRawParameterValue("delaytime");
	m_FeedbackParameter = tree.getRawParameterValue("feedback");
	m_DryWetParameter = tree.getRawParameterValue("drywet");
	m_CutOff = tree.getRawParameterValue("cutoff");	
}

DelayAudioProcessor::~DelayAudioProcessor()
{
	delete m_filterBlock;
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

	m_sampleRate = sampleRate;

	//two seconds of delay time plus little bit of extra room
	const int delayBufferSize = 2 * (m_sampleRate + samplesPerBlock);

	m_DelayBuffer.setSize(getTotalNumInputChannels(), delayBufferSize);
	m_DelayBuffer.clear();

	m_filterBuffer.setSize(getTotalNumInputChannels(), 2 * samplesPerBlock);
	m_filterBuffer.clear();

	m_filterBlock = new juce::dsp::AudioBlock<float>(m_filterBuffer);

	dsp::ProcessSpec spec;
	spec.sampleRate = m_sampleRate;
	spec.maximumBlockSize = 2 * samplesPerBlock;
	spec.numChannels = getTotalNumInputChannels(); //getTotalNumOutputChannels(); ???

	m_filter1.prepare(spec);
	m_filter1.setCutoffFrequencyHz(*m_CutOff);

	m_filter2.prepare(spec);
	m_filter2.setCutoffFrequencyHz(*m_CutOff);
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

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.

	m_filter1.setCutoffFrequencyHz(*m_CutOff);
	m_filter2.setCutoffFrequencyHz(*m_CutOff);

	DelayBufferInput(totalNumInputChannels, buffer);
	DelayBufferFeedback(totalNumInputChannels, buffer);
	DWMix(totalNumInputChannels, buffer);

	//shift delay buffer write position by main buffer's length
	m_delayBufferWritePosition += buffer.getNumSamples();
	//if write position exceeds delay buffer length, wrap around
	m_delayBufferWritePosition %= m_DelayBuffer.getNumSamples();

}

//Copy bufferData to delayBufferData at m_delayBufferWritePosition
void DelayAudioProcessor::DelayBufferInput(int channels, AudioBuffer<float>& buffer)
{
	int delayBufferLength = m_DelayBuffer.getNumSamples();
	int bufferLength = buffer.getNumSamples();

	for (int ch = 0; ch < channels; ++ch)
	{
		//auto bufferData = buffer.getReadPointer(ch);
		m_filterBuffer.copyFrom(ch, 0, buffer, ch, 0, bufferLength);
	}

	m_filter1.process(juce::dsp::ProcessContextReplacing<float>(m_filterBlock->getSubBlock(0, bufferLength)));

	//number of empty samples left in delay buffer:
	//(last_sample_idx - first_empty_sample_idx) + 1 = last_sample_idx + 1 - first_empty_sample_idx = delay_buffer_length - first_empty_sample_idx
	const int emptySamplesLeft = delayBufferLength - m_delayBufferWritePosition;

	for (int ch = 0; ch < channels; ++ch)
	{
		//is there in delay buff enough space to fit temp buff?
		if (emptySamplesLeft >= bufferLength)
		{
			//there is
			m_DelayBuffer.copyFrom(ch, m_delayBufferWritePosition, m_filterBuffer, ch, 0, bufferLength);
		}
		else
		{
			//there is not
			m_DelayBuffer.copyFrom(ch, m_delayBufferWritePosition, m_filterBuffer, ch, 0, emptySamplesLeft);
			m_DelayBuffer.copyFrom(ch, 0, m_filterBuffer, ch, emptySamplesLeft, bufferLength - emptySamplesLeft);
		}
	}
}

//Add delay buffer's output to it's input
void DelayAudioProcessor::DelayBufferFeedback(int channels, AudioBuffer<float>& buffer)
{
	auto bufferLength = buffer.getNumSamples();
	auto delayBufferLength = m_DelayBuffer.getNumSamples();

	//delay time in ms
	const int delayTime = *m_DelayTimeParameter;
	const float feedback = *m_FeedbackParameter;

	//sample rate = samples per second
	const int samplesPerDelayTime = std::lround(m_sampleRate * (delayTime / 1000.0));
	//go back in time
	m_delayedBlockPosition = m_delayBufferWritePosition - samplesPerDelayTime; // -1 ???
	if (m_delayedBlockPosition < 0)
	{
		m_delayedBlockPosition = delayBufferLength + m_delayedBlockPosition;
	}

	for (int ch = 0; ch < channels; ++ch)
	{
		for (int i = 0; i < bufferLength; i++)
		{
			auto delayedSamplePosition = (m_delayedBlockPosition + i) % delayBufferLength;
			auto delayedSample = m_DelayBuffer.getSample(ch, delayedSamplePosition);
			m_filterBuffer.setSample(ch, i, delayedSample);
		}
	}

	m_filter2.process(juce::dsp::ProcessContextReplacing<float>(m_filterBlock->getSubBlock(0, bufferLength)));

	for (int ch = 0; ch < channels; ++ch)
	{
		for (int i = 0; i < bufferLength; i++)
		{
			auto currentSamplePosition = (m_delayBufferWritePosition + i) % delayBufferLength;
			auto currentSample = m_DelayBuffer.getSample(ch, currentSamplePosition);
			m_DelayBuffer.setSample(ch, currentSamplePosition, (feedback * m_filterBuffer.getSample(ch, i)) + currentSample);
		}
	}
}

void DelayAudioProcessor::DWMix(int channels, AudioBuffer<float>& buffer)
{
	float dryWet = *m_DryWetParameter;
	int bufferLength = buffer.getNumSamples();
	int delayBufferLength = m_DelayBuffer.getNumSamples();

	for (int ch = 0; ch < channels; ++ch)
	{
		auto delayBufferData = m_DelayBuffer.getReadPointer(ch);
		buffer.applyGain(ch, 0, bufferLength, 1.0f - dryWet);
		const int numOfRemainingSamplesInDelayBuff = delayBufferLength - m_delayedBlockPosition;

		if (numOfRemainingSamplesInDelayBuff >= bufferLength)
		{
			buffer.addFrom(ch, 0, delayBufferData + m_delayedBlockPosition, bufferLength, dryWet);
		}
		else
		{
			buffer.addFrom(ch, 0, delayBufferData + m_delayedBlockPosition, numOfRemainingSamplesInDelayBuff, dryWet);
			buffer.addFrom(ch, 0 + numOfRemainingSamplesInDelayBuff, delayBufferData, bufferLength - numOfRemainingSamplesInDelayBuff, dryWet);
		}
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
