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
	setSize(500, 300);

	m_sldDelayTime.setSliderStyle(Slider::RotaryVerticalDrag);
	m_sldDelayTime.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	m_sldDelayTime.setTextValueSuffix(" ms");
	addAndMakeVisible(m_sldDelayTime);

	m_sldFeedback.setSliderStyle(Slider::RotaryVerticalDrag);
	m_sldFeedback.setTextBoxStyle(Slider::TextBoxBelow, true, 90, 20);
	m_sldFeedback.setPopupDisplayEnabled(true, false, this);
	addAndMakeVisible(m_sldFeedback);

	//mChangeDelayTimeButton.setButtonText("change delay time");
	//addAndMakeVisible(&mChangeDelayTimeButton);

	mBtnReset.setButtonText("reset");
	mBtnReset.addListener(this);
	addAndMakeVisible(mBtnReset);
	
	m_lblDelayTime.setText("Delay time", NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblDelayTime);
	m_lblFeedback.setText("Feedback", NotificationType::dontSendNotification);
	addAndMakeVisible(m_lblFeedback);

	m_attachDelayTime.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "delaytime", m_sldDelayTime));
	m_attachFeedback.reset(new AudioProcessorValueTreeState::SliderAttachment(processor.tree, "feedback", m_sldFeedback));
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
	
	//mChangeDelayTimeButton.setBounds(80, 30, 50, 30);
	mBtnReset.setBounds(getWidth() - 50 - 10, 10, 50, 30);
}

void DelayAudioProcessorEditor::buttonClicked(Button* btn)
{
	auto text = btn->getButtonText();
	if (text == "change delay time")
	{
		processor.mDelayTime -= 100;
	}
	else if (text == "reset")
	{
		setSize(500, 300);
		m_sldDelayTime.setValue(250);
		m_sldFeedback.setValue(0.6);
		//processor.mDelayTime = 500;
	}
}
