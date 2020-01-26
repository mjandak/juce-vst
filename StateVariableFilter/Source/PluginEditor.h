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
class StateVariableFilterAudioProcessorEditor  : public AudioProcessorEditor
{
public:
	ComboBox cbxFilterType;
	Label lblCutoff;
	Slider sldCutOff;
	Label lblRes;
	Slider sldRes;
	
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttach;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> cutOffAttach;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> resAttach;

    StateVariableFilterAudioProcessorEditor (StateVariableFilterAudioProcessor&);
    ~StateVariableFilterAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StateVariableFilterAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StateVariableFilterAudioProcessorEditor)
};
