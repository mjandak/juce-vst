/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//using Parameter = AudioProcessorValueTreeState::Parameter;
//==============================================================================
StateVariableFilterAudioProcessor::StateVariableFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	), tree(*this, nullptr, "PARAMETERS", {
		std::make_unique<AudioParameterInt>("filterType", "Filter Menu", 0, 2, 0),
		std::make_unique<AudioParameterFloat>("cutoff", "Freq", NormalisableRange<float>(20.0, 20000.0, 0.0, 0.25), 600.0),
		std::make_unique<AudioParameterFloat>("res", "Res", NormalisableRange<float>(0.0, 5.0), 1.0 / MathConstants<double>::sqrt2) })
#endif
{
	//addParameter(mFilterTypeParam = new AudioParameterInt(
	//	"filterType", // parameter ID
	//	"FilterType", // parameter name
	//	0,   // minimum value
	//	2,   // maximum value
	//	0)); // default value
	//addParameter(mCutOffParam = new AudioParameterFloat(
	//	"cutOff", // parameter ID
	//	"CutOff", // parameter name
	//	0,   // minimum value
	//	1,   // maximum value
	//	0.4)); // default value

	filerTypeParameter = tree.getRawParameterValue("filterType");
	cutoffParameter = tree.getRawParameterValue("cutoff");
	resParameter = tree.getRawParameterValue("res");
}

StateVariableFilterAudioProcessor::~StateVariableFilterAudioProcessor()
{
}

//==============================================================================
const String StateVariableFilterAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool StateVariableFilterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool StateVariableFilterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool StateVariableFilterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double StateVariableFilterAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int StateVariableFilterAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int StateVariableFilterAudioProcessor::getCurrentProgram()
{
	return 0;
}

void StateVariableFilterAudioProcessor::setCurrentProgram(int index)
{
}

const String StateVariableFilterAudioProcessor::getProgramName(int index)
{
	return {};
}

void StateVariableFilterAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void StateVariableFilterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getMainBusNumOutputChannels();

	stateVarialbleFilter.reset();
	updateFilter();
	stateVarialbleFilter.prepare(spec);
}

void StateVariableFilterAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StateVariableFilterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void StateVariableFilterAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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

	dsp::AudioBlock<float> block(buffer);
	updateFilter();
	stateVarialbleFilter.process(dsp::ProcessContextReplacing<float>(block));

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
	//for (int channel = 0; channel < totalNumInputChannels; ++channel)
	//{
	//    auto* channelData = buffer.getWritePointer (channel);

	//    // ..do something to the data...
	//}
}

void StateVariableFilterAudioProcessor::updateFilter()
{
	if (*filerTypeParameter == 0)
	{
		stateVarialbleFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
		stateVarialbleFilter.state->setCutOffFrequency(getSampleRate(), *cutoffParameter, *resParameter);
	}
	else if (*filerTypeParameter == 1)
	{
		stateVarialbleFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::bandPass;
		stateVarialbleFilter.state->setCutOffFrequency(getSampleRate(), *cutoffParameter, *resParameter);
	}
	else if (*filerTypeParameter == 2)
	{
		stateVarialbleFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::highPass;
		stateVarialbleFilter.state->setCutOffFrequency(getSampleRate(), *cutoffParameter, *resParameter);
	}
}

//==============================================================================
bool StateVariableFilterAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StateVariableFilterAudioProcessor::createEditor()
{
	return new StateVariableFilterAudioProcessorEditor(*this);
}

//==============================================================================
void StateVariableFilterAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void StateVariableFilterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new StateVariableFilterAudioProcessor();
}
