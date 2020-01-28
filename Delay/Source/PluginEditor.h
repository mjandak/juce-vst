/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DelayAudioProcessorEditor : public AudioProcessorEditor, TextButton::Listener
{
public:
	DelayAudioProcessorEditor(DelayAudioProcessor&);
	~DelayAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	DelayAudioProcessor& processor;

	Label m_lblDelayTime;
	Slider m_sldDelayTime;
	Label m_lblFeedback;
	Slider m_sldFeedback;
	Slider m_sldDryWet;

	TextButton mBtnReset;
	//TextButton mChangeDelayTimeButton;

	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m_attachDelayTime;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m_attachFeedback;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> m_attachDryWet;

	void buttonClicked(Button*) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayAudioProcessorEditor)
};
