/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StateVariableFilterAudioProcessorEditor::StateVariableFilterAudioProcessorEditor(StateVariableFilterAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(400, 300);

	cbxFilterType.setJustificationType(Justification::centred);
	cbxFilterType.addItem("low pass", 1);
	cbxFilterType.addItem("band pass", 2);
	cbxFilterType.addItem("high pass", 3);
	addAndMakeVisible(&cbxFilterType);

	sldCutOff.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	sldCutOff.setName("cut off freq");
	sldCutOff.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	addAndMakeVisible(&sldCutOff);

	sldRes.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	sldRes.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	addAndMakeVisible(&sldRes);

	filterTypeAttach.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(processor.tree, "filterType", cbxFilterType));
	cutOffAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "cutoff", sldCutOff));
	resAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "res", sldRes));

	addAndMakeVisible(lblCutoff);
	lblCutoff.setText("Freq", NotificationType::dontSendNotification);

	addAndMakeVisible(lblRes);
	lblRes.setText("Res", NotificationType::dontSendNotification);

}

StateVariableFilterAudioProcessorEditor::~StateVariableFilterAudioProcessorEditor()
{
}

//==============================================================================
void StateVariableFilterAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void StateVariableFilterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	setSize(500, 400);

	cbxFilterType.setBounds(10, 10, 150, 20);
	
	lblCutoff.setBounds(60-25, 40, 50, 20);
	sldCutOff.setBounds(10, 60, 100, 100);

	lblRes.setBounds(170-25, 40, 50, 20);
	sldRes.setBounds(120, 60, 100, 100);
	
}
