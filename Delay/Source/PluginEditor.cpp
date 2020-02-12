/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayAudioProcessorEditor::DelayAudioProcessorEditor(DelayAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(600, 200);

	m_sldDelayTime.setSliderStyle(Slider::RotaryVerticalDrag);
	m_sldDelayTime.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	m_sldDelayTime.setTextValueSuffix(" ms");
	addAndMakeVisible(m_sldDelayTime);

	m_sldFeedback.setSliderStyle(Slider::RotaryVerticalDrag);
	m_sldFeedback.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	addAndMakeVisible(m_sldFeedback);

	m_sldDryWet.setSliderStyle(Slider::RotaryVerticalDrag);
	m_sldDryWet.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	addAndMakeVisible(m_sldDryWet);

	m_sldFeedbackFilterFreq.setSliderStyle(Slider::RotaryVerticalDrag);
	m_sldFeedbackFilterFreq.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	m_sldFeedbackFilterFreq.setTextValueSuffix(" Hz");
	addAndMakeVisible(m_sldFeedbackFilterFreq);

	//mChangeDelayTimeButton.setButtonText("change delay time");
	//addAndMakeVisible(&mChangeDelayTimeButton);

	mBtnReset.setButtonText("reset");
	mBtnReset.addListener(this);
	addAndMakeVisible(mBtnReset);
	
	m_lblDelayTime.setText("Delay time", NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblDelayTime);
	m_lblFeedback.setText("Feedback", NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblFeedback);
	m_lblDryWet.setText("Dry/Wet", NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblDryWet);
	m_lblFeedbackFilterFreq.setText("Feedback filter", NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblFeedbackFilterFreq);

	m_attachDelayTime.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "delaytime", m_sldDelayTime));
	m_attachFeedback.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "feedback", m_sldFeedback));
	m_attachDryWet.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "drywet", m_sldDryWet));
	m_attachFeedbackFilterFreq.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "cutoff", m_sldFeedbackFilterFreq));
}

DelayAudioProcessorEditor::~DelayAudioProcessorEditor()
{
}

//==============================================================================
void DelayAudioProcessorEditor::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

	g.setColour(Colours::white);
	g.setFont(15.0f);
	//g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void DelayAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..

	m_lblDelayTime.setBounds(10, 10, 100, 20);
	m_lblDelayTime.setJustificationType(Justification::centred);
	m_sldDelayTime.setBounds(10, 40, 100, 100);
	
	m_lblFeedback.setBounds(120, 10, 100, 20);
	m_lblFeedback.setJustificationType(Justification::centred);
	m_sldFeedback.setBounds(120, 40, 100, 100);

	m_lblFeedbackFilterFreq.setBounds(230, 10, 100, 20);
	m_lblFeedbackFilterFreq.setJustificationType(Justification::centred);
	m_sldFeedbackFilterFreq.setBounds(230, 40, 100, 100);

	m_lblDryWet.setBounds(340, 10, 100, 20);
	m_lblDryWet.setJustificationType(Justification::centred);
	m_sldDryWet.setBounds(340, 40, 100, 100);
	
	//mChangeDelayTimeButton.setBounds(80, 30, 50, 30);
	mBtnReset.setBounds(getWidth() - 50 - 10, 10, 50, 30);
}

void DelayAudioProcessorEditor::buttonClicked(Button* btn)
{
	auto text = btn->getButtonText();
	if (text == "change delay time")
	{
		//processor.mDelayTime -= 100;
	}
	else if (text == "reset")
	{
		setSize(600, 200);
		m_sldDelayTime.setValue(m_sldDelayTime.getDoubleClickReturnValue());
		m_sldFeedback.setValue(m_sldFeedback.getDoubleClickReturnValue());
		m_sldDryWet.setValue(m_sldDryWet.getDoubleClickReturnValue());
		m_sldFeedbackFilterFreq.setValue(m_sldFeedbackFilterFreq.getDoubleClickReturnValue());
		//processor.mDelayTime = 500;
	}
}
