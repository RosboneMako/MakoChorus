/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MakoBiteAudioProcessorEditor::MakoBiteAudioProcessorEditor (MakoBiteAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //R1.00 Create SLIDER ATTACHMENTS so our parameter vars get adjusted automatically for Get/Set states.
    ParAtt[e_Gain1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,  "gain1",  sldKnob[e_Gain1]);
    ParAtt[e_Start1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "start1", sldKnob[e_Start1]);
    ParAtt[e_Stop1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,  "stop1",  sldKnob[e_Stop1]);
    ParAtt[e_Rate1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,  "rate1",  sldKnob[e_Rate1]);
    ParAtt[e_Mix1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "mix1",   sldKnob[e_Mix1]);
    ParAtt[e_Start2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "start2", sldKnob[e_Start2]);
    ParAtt[e_Stop2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,  "stop2",  sldKnob[e_Stop2]);
    ParAtt[e_Rate2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,  "rate2",  sldKnob[e_Rate2]);
    ParAtt[e_Mix2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "mix2",   sldKnob[e_Mix2]);
    ParAtt[e_LP1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,    "lp1",    sldKnob[e_LP1]);
    ParAtt[e_Bal1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "bal1",   sldKnob[e_Bal1]);
    ParAtt[e_LP2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,    "lp2",    sldKnob[e_LP2]);
    ParAtt[e_Bal2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,   "bal2",   sldKnob[e_Bal2]);
    ParAtt[e_Mode1] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mode1", sldKnob[e_Mode1]);
    ParAtt[e_Mode2] = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mode2", sldKnob[e_Mode2]);
    ParAtt_Mono = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "mono", jsP1_Mono);
    ParAtt_Chor2Enable = std::make_unique <juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, "chor2enable", jsP1_Chor2Enable);
        

    //R1.00 PRESET COMBO BOX Def.
    cbPreset.setColour(juce::ComboBox::textColourId, juce::Colour(192, 192, 192));
    cbPreset.setColour(juce::ComboBox::backgroundColourId, juce::Colour(32, 32, 32));
    cbPreset.setColour(juce::ComboBox::arrowColourId, juce::Colour(192, 192, 192));
    cbPreset.setColour(juce::ComboBox::outlineColourId, juce::Colour(10, 100, 150));
    addAndMakeVisible(cbPreset);
    //cbPreset.addSectionHeading("Cool Header");
    cbPreset.addItem("---", 1);
    cbPreset.addItem("Chorus", 2);
    cbPreset.addItem("Flange", 3);
    cbPreset.addItem("Spleen", 4);
    cbPreset.addItem("Colon", 5);
    cbPreset.addItem("Duane", 6);
    cbPreset.onChange = [this] { cbPresetChanged(); };    //R1.00 PresetChanged is a func we create and gets called on combo selection.
    cbPreset.setSelectedId(1);

    //****************************************************************************************
    //R1.00 ADD GUI CONTROLS
    //****************************************************************************************
    //R1.00 Large Rotary Slider
    GUI_Init_Large_Slider(&sldKnob[e_Gain1],  audioProcessor.Setting[e_Gain1],  0.0f,  2.0f, .01f,"");     //R1.00 Gain 1
    GUI_Init_Large_Slider(&sldKnob[e_Start1], audioProcessor.Setting[e_Start1], 0.1f, 50.0f, .01f, " mS"); //R1.00 Start 1
    GUI_Init_Large_Slider(&sldKnob[e_Stop1],  audioProcessor.Setting[e_Stop1],  0.1f, 50.0f, .01f, " mS"); //R1.00 Stop 1
    GUI_Init_Large_Slider(&sldKnob[e_Rate1],  audioProcessor.Setting[e_Rate1],  0.01f, 1.0f, .01f, "");    //R1.00 Rate 1
    GUI_Init_Large_Slider(&sldKnob[e_Mix1],   audioProcessor.Setting[e_Mix1],   0.0f,  1.0f, .01f, "");    //R1.00 Mix 1
    GUI_Init_Large_Slider(&sldKnob[e_Start2], audioProcessor.Setting[e_Start2], 0.1f, 50.0f, .01f, " mS"); //R1.00 Start 2
    GUI_Init_Large_Slider(&sldKnob[e_Stop2],  audioProcessor.Setting[e_Stop2],  0.1f, 50.0f, .01f, " mS"); //R1.00 Stop 2
    GUI_Init_Large_Slider(&sldKnob[e_Rate2],  audioProcessor.Setting[e_Rate2], 0.01f, 1.0f, .01f, "");     //R1.00 Rate 2
    GUI_Init_Large_Slider(&sldKnob[e_Mix2],   audioProcessor.Setting[e_Mix2], 0.0f, 1.0f, .01f, "");       //R1.00 Mix 2
    GUI_Init_Large_Slider(&sldKnob[e_LP1],    audioProcessor.Setting[e_LP1], 100, 8000, 25, " Hz");        //R1.00 LP 1
    GUI_Init_Large_Slider(&sldKnob[e_Bal1],   audioProcessor.Setting[e_Bal1], 0.0f, 1.0f, .01f, "");       //R1.00 Bal 1
    GUI_Init_Large_Slider(&sldKnob[e_LP2],    audioProcessor.Setting[e_LP2], 100, 8000, 25, " Hz");        //R1.00 LP 2
    GUI_Init_Large_Slider(&sldKnob[e_Bal2],   audioProcessor.Setting[e_Bal2], 0.0f, 1.0f, .01f, "");       //R1.00 Bal 2 
    GUI_Init_Large_Slider(&sldKnob[e_Mode1],  audioProcessor.Setting[e_Mode1], 0, 6, 1, "");               //R1.00 Mode 1
    GUI_Init_Large_Slider(&sldKnob[e_Mode2],  audioProcessor.Setting[e_Mode2], 0, 6, 1, "");               //R1.00 Mode 2 

    //R1.00 Keep track of how many knobs we have setup.
    Knob_Cnt = 15;

    //R1.00 If the second chorus is turned off, make the sliders invisible.
    if (!audioProcessor.Pedal_Chor2Enable)
    {
        sldKnob[e_Start2].setEnabled(false);
        sldKnob[e_Stop2].setEnabled(false);
        sldKnob[e_Rate2].setEnabled(false);
        sldKnob[e_Mix2].setEnabled(false);
        sldKnob[e_LP2].setEnabled(false);
        sldKnob[e_Bal2].setEnabled(false);
        sldKnob[e_Mode2].setEnabled(false);
    }

    //R1.00 Setup the small option sliders.
    GUI_Init_Small_Slider(&jsP1_Chor2Enable, audioProcessor.Pedal_Chor2Enable, 0, 1, 1, "");
    GUI_Init_Small_Slider(&jsP1_Mono, audioProcessor.Pedal_Mono, 0, 1, 1, "");
    GUI_Init_Small_Slider(&jsP1_KnobStyle, audioProcessor.Pedal_Mono, 0, 3, 1, "");
    jsP1_KnobStyle.setValue(otherLookAndFeel.MakoSliderKnobStyle,juce::dontSendNotification);

    //R1.00 Update the Look and Feel (Global colors) so drop down menu is the correct color. 
    getLookAndFeel().setColour(juce::DocumentWindow::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::DocumentWindow::textColourId, juce::Colour(255, 255, 255));
    getLookAndFeel().setColour(juce::DialogWindow::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(10, 100, 150));
    getLookAndFeel().setColour(juce::TextButton::buttonOnColourId, juce::Colour(10, 100, 150));
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::ComboBox::backgroundColourId, juce::Colour(0, 0, 0));
    getLookAndFeel().setColour(juce::ListBox::backgroundColourId, juce::Colour(32, 32, 32));
    getLookAndFeel().setColour(juce::Label::backgroundColourId, juce::Colour(32, 32, 32));

    //R1.00 Help Text object. 
    labHelp.setText("", juce::dontSendNotification);
    labHelp.setJustificationType(juce::Justification::centred);
    labHelp.setColour(juce::Label::backgroundColourId, juce::Colour(32, 32, 32));
    labHelp.setColour(juce::Label::textColourId, juce::Colour(192, 192, 192));
    labHelp.setColour(juce::Label::outlineColourId, juce::Colour(10, 50, 150));
    addAndMakeVisible(labHelp);
    labHelp.setText("Mako CHORUS Version 1.0", juce::dontSendNotification);

    //R1.00 Define the knob (slider) positions on the screen/UI.
    KNOB_DefinePosition(0,   30, 48, 70, 70, "Gain"); 

    KNOB_DefinePosition(1,  125, 20, 70, 80, "Start 1");
    KNOB_DefinePosition(2,  200, 20, 70, 80, "Stop 1");
    KNOB_DefinePosition(3,  275, 20, 70, 80, "Rate 1");
    KNOB_DefinePosition(4,  350, 20, 70, 80, "Mix 1");
    KNOB_DefinePosition(9,  425, 20, 70, 80, "Hi Cut 1");
    KNOB_DefinePosition(10, 500, 20, 70, 80, "Bal 1");
    KNOB_DefinePosition(13, 575, 20, 70, 80, "Mode 1");

    KNOB_DefinePosition(5,  125, 130, 70, 80, "Start 2");
    KNOB_DefinePosition(6,  200, 130, 70, 80, "Stop 2");
    KNOB_DefinePosition(7,  275, 130, 70, 80, "Rate 2");
    KNOB_DefinePosition(8,  350, 130, 70, 80, "Mix 2");
    KNOB_DefinePosition(11, 425, 130, 70, 80, "Hi Cut 2");
    KNOB_DefinePosition(12, 500, 130, 70, 80, "Bal 2");
    KNOB_DefinePosition(14, 575, 130, 70, 80, "Mode 2");

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
     
    //R1.00 Set the window size.
    setSize(655, 250);
}


MakoBiteAudioProcessorEditor::~MakoBiteAudioProcessorEditor()
{
}

//==============================================================================
void MakoBiteAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient ColGrad;

    //R1.00 Draw our GUI.
    //R1.00 Background.
    ColGrad = juce::ColourGradient(juce::Colour(0xFF606068), 0.0f, 0.0f, juce::Colour(0xFFE0E0E0), 0.0f, 100.0f, false);
    g.setGradientFill(ColGrad);
    g.fillRect(0, 0, 655, 100);
    ColGrad = juce::ColourGradient(juce::Colour(0xFFE0E0E0), 0.0f, 100.0f, juce::Colour(0xFFB0B0B8), 0.0f, 250.0f, false);
    g.setGradientFill(ColGrad);
    g.fillRect(0, 100, 655, 150);

    //R1.00 Headers.
    g.setColour(juce::Colour(0xFF0568AD));
    for (int t = 0; t < Knob_Cnt; t++) g.fillRect(Knob_Pos[t].x, Knob_Pos[t].y - 15.0f, Knob_Pos[t].sizex, 15.0f);
    g.setFont(12.0f);
    g.setColour(juce::Colours::white); 
    for (int t = 0; t < Knob_Cnt; t++) g.drawFittedText(Knob_Name[t], Knob_Pos[t].x, Knob_Pos[t].y - 15, Knob_Pos[t].sizex, 15, juce::Justification::centred, 1);

    g.setColour(juce::Colour(0xFF0568AD));
    g.drawFittedText("Chor 2 Enable", 0, 125, 130, 15, juce::Justification::centred, 1);
    g.drawFittedText("Stereo/Mono", 0, 155, 130, 15, juce::Justification::centred, 1);
    g.drawFittedText("Knob Style", 0, 185, 130, 15, juce::Justification::centred, 1);

    g.setColour(juce::Colour(0x20000000));
    g.fillRect(10, 2, 110, 210);
    
    g.setColour(juce::Colour(0xFFA0A0A0));
    g.setFont(11.0f);
    g.drawFittedText("m a k o", 0, 14, 130, 15, juce::Justification::centred, 1);
    g.setColour(juce::Colour(0xFFFFFFFF));
    g.setFont(16.0f);
    g.drawFittedText("CHORUS", 0, 4, 130, 15, juce::Justification::centred, 1);    
}

void MakoBiteAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //R1.00 Draw all of the defined KNOBS.
    for (int t = 0; t < Knob_Cnt;t++) sldKnob[t].setBounds(Knob_Pos[t].x, Knob_Pos[t].y, Knob_Pos[t].sizex, Knob_Pos[t].sizey);
    
    //R1.00 Add some switches (Sliders).
    jsP1_Chor2Enable.setBounds(40, 130, 50, 30); 
    jsP1_Mono.setBounds(40, 160, 50, 30);    
    jsP1_KnobStyle.setBounds(40, 190, 50, 30);

    cbPreset.setBounds  (10, 220, 110, 18);    //R1.00 Preset Dropdown List.
    labHelp.setBounds  (125, 220, 520, 18);    //R1.00 Help Text / status bar.
}

void MakoBiteAudioProcessorEditor::cbPresetChanged()
{
    //R1.00 User selected one of our premade sounds.
    int Sel = cbPreset.getSelectedId() - 1;
    if (Sel == 0) return;

    switch (Sel)
    {
        case 1:  Preset01(); break;   //R1.00 Chorus.
        case 2:  Preset02(); break;   //R1.00 Flange.
        case 3:  Preset03(); break;   //R1.00 Spleen.    
        case 4:  Preset04(); break;   //R1.00 Colon.    
        case 5:  Preset05(); break;   //R1.00 Duane.    
    }

    //R1.00 Flag the processor, we need to update settings.
    audioProcessor.SettingsChanged = true;    
}

void MakoBiteAudioProcessorEditor::cbPreset_UpdateSelection(int button, int idx, int editmode)
{    
}

void MakoBiteAudioProcessorEditor::Preset01()
{
    //R1.00 CHORUS
    jsP1_Chor2Enable.setValue(false, juce::sendNotification);
    sldKnob[e_Start1].setValue(16.5f, juce::sendNotification); //R1.00 1/.0165 = 60.6 Hz
    sldKnob[e_Stop1].setValue(19.5f, juce::sendNotification);  //R1.00 1/.0195 = 51.3 Hz
    sldKnob[e_Rate1].setValue(.2f, juce::sendNotification);
    sldKnob[e_Mix1].setValue(.5f, juce::sendNotification);
    sldKnob[e_LP1].setValue(8000, juce::sendNotification); 
    sldKnob[e_Bal1].setValue(.5f, juce::sendNotification); 
    sldKnob[e_Mode1].setValue(0, juce::sendNotification);

    //R1.00 Stereo sounds best.
    jsP1_Mono.setValue(0.0f);
}

void MakoBiteAudioProcessorEditor::Preset02()
{
    //R1.00 Setup a FLANGER. We want to sweep a comb filter (delay)
    //R1.00 between 200 and 2000 Hz.
    jsP1_Chor2Enable.setValue(false, juce::sendNotification);
    sldKnob[e_Start1].setValue(.5f, juce::sendNotification);  //R1.00 1/.0005 = 2000 Hz
    sldKnob[e_Stop1].setValue(5.0f, juce::sendNotification);  //R1.00 1/.005 = 200 Hz
    sldKnob[e_Rate1].setValue(.05f, juce::sendNotification);
    sldKnob[e_Mix1].setValue(.5f, juce::sendNotification);
    sldKnob[e_LP1].setValue(8000, juce::sendNotification); 
    sldKnob[e_Bal1].setValue(.5f, juce::sendNotification); 
    sldKnob[e_Mode1].setValue(1, juce::sendNotification);

    //R1.00 Flanger sounds best in MONO.
    //R1.00 Wont hear the comb filter freq cancelling in stereo because
    //R1.00 the other speaker will be playing the missing freq.    
    jsP1_Mono.setValue(1.0f);                                 
}

void MakoBiteAudioProcessorEditor::Preset03()
{
    //R1.00 SPLEEN - Organ type pitch effect.
    //R1.00 Rate is not used. Start/Stop dictate the modulation rate.
    jsP1_Chor2Enable.setValue(false, juce::sendNotification);
    sldKnob[e_Start1].setValue(.1f, juce::sendNotification);
    sldKnob[e_Stop1].setValue(50.0f, juce::sendNotification);
    sldKnob[e_Rate1].setValue(.4f, juce::sendNotification);   //R1.00 Rate is not used.
    sldKnob[e_Mix1].setValue(.5f, juce::sendNotification);
    sldKnob[e_LP1].setValue(8000, juce::sendNotification); 
    sldKnob[e_Bal1].setValue(.5f, juce::sendNotification); 
    sldKnob[e_Mode1].setValue(4, juce::sendNotification);
}

void MakoBiteAudioProcessorEditor::Preset04()
{
    //R1.00 COLON - Organ type pitch effect.
    //R1.00 Rate is not used. Start/Stop dictate the modulation rate.
    jsP1_Chor2Enable.setValue(false, juce::sendNotification);
    sldKnob[e_Start1].setValue(.1f, juce::sendNotification);
    sldKnob[e_Stop1].setValue(50.0f, juce::sendNotification);
    sldKnob[e_Rate1].setValue(.4f, juce::sendNotification);   //R1.00 Rate is not used.
    sldKnob[e_Mix1].setValue(.5f, juce::sendNotification);
    sldKnob[e_LP1].setValue(150, juce::sendNotification);
    sldKnob[e_Bal1].setValue(.5f, juce::sendNotification);
    sldKnob[e_Mode1].setValue(5, juce::sendNotification);
}

void MakoBiteAudioProcessorEditor::Preset05()
{
    //R1.00 COLON - Organ type pitch effect.
    //R1.00 Rate is not used. Start/Stop dictate the modulation rate.
    jsP1_Chor2Enable.setValue(false, juce::sendNotification);
    sldKnob[e_Start1].setValue(.1f, juce::sendNotification);
    sldKnob[e_Stop1].setValue(50.0f, juce::sendNotification);
    sldKnob[e_Rate1].setValue(.4f, juce::sendNotification);   //R1.00 Rate is not used.
    sldKnob[e_Mix1].setValue(.16f, juce::sendNotification);
    sldKnob[e_LP1].setValue(4500, juce::sendNotification);
    sldKnob[e_Bal1].setValue(.5f, juce::sendNotification);
    sldKnob[e_Mode1].setValue(6, juce::sendNotification);
}


void MakoBiteAudioProcessorEditor::GUI_Init_Large_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix)
{   
    //R1.00 Setup the slider edit parameters.
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider->setTextValueSuffix(Suffix);
    slider->setRange(Vmin, Vmax, Vinterval);
    slider->setValue(Val);    
    slider->addListener(this);
    addAndMakeVisible(slider);    

    //R1.00 Override the default Juce drawing routines and use ours.
    slider->setLookAndFeel(&otherLookAndFeel);

    //R1.00 Setup the type and colors for the sliders.
    slider->setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFA0F0F0));
    slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF202020));
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(10,100,150));
    slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0x00000000));    //R1.00 Make this SEE THRU. Alpha=0.
    slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0x00000000)); //R1.00 Make this SEE THRU. Alpha=0.
    slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xFF000000));
}

void MakoBiteAudioProcessorEditor::GUI_Init_Small_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix)
{
    //R1.00 Setup the slider edit parameters.
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    slider->setRange(Vmin, Vmax, Vinterval);
    slider->setTextValueSuffix(Suffix);
    slider->setValue(Val);
    slider->addListener(this);
    addAndMakeVisible(slider);    

    //R1.00 Setup the type and colors for the sliders.
    slider->setSliderStyle(juce::Slider::LinearHorizontal);
    slider->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFA0A0A0));
    slider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF202020));
    slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0, 255, 255));
    slider->setColour(juce::Slider::trackColourId, juce::Colour(0, 255, 255));
    slider->setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF000000));
    slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xFFF0F0F0));
}

void MakoBiteAudioProcessorEditor::KNOB_DefinePosition(int idx,float x, float y, float sizex, float sizey, juce::String name)
{
    //R1.00 Define our knob positions so we can just loop thru and draw them all easily.
    Knob_Pos[idx].x = x;
    Knob_Pos[idx].y = y;
    Knob_Pos[idx].sizex = sizex;
    Knob_Pos[idx].sizey = sizey;
    Knob_Name[idx] = name;
}

void MakoBiteAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{   
    float v1;
    float v2;

    //R1.00 Check which slider has been adjusted.
    for (int t = 0; t < Knob_Cnt; t++)
    {
        if (slider == &sldKnob[t])
        {
            //R1.00 Update HELP bar.
            labHelp.setText(HelpString[t], juce::dontSendNotification);

            //R1.00 Update the actual processor variable being edited.
            audioProcessor.Setting[t] = float(sldKnob[t].getValue());
            
            //R1.00 We need to update settings in processor. Unless we are making large changes in code (100).
            //R1.00 Increment changed var to be sure, every change gets made, changed var is decremented in processor.
            //if (audioProcessor.SettingsType != 100)
            audioProcessor.SettingsChanged += 1; 

            //R1.00 We have captured the correct slider change, exit this function.
            return;
        }
    }

    //R1.00 Stereo MONO.
    if (slider == &jsP1_Mono)
    {
        labHelp.setText("Toggle Stereo or Mono operation.", juce::dontSendNotification);
        audioProcessor.Pedal_Mono = float(jsP1_Mono.getValue());
        return;
    }

    //R1.00 If Chorus 2 is turned off, disable Chorus 2 controls.
    if (slider == &jsP1_Chor2Enable)
    {
        labHelp.setText("Toggle Chorus effect #2 On/Off.", juce::dontSendNotification);
        audioProcessor.Pedal_Chor2Enable = float(jsP1_Chor2Enable.getValue());
        
        bool tB = false;
        if (audioProcessor.Pedal_Chor2Enable) tB = true;
        sldKnob[e_Start2].setEnabled(tB);
        sldKnob[e_Stop2].setEnabled(tB);
        sldKnob[e_Rate2].setEnabled(tB);
        sldKnob[e_Mix2].setEnabled(tB);
        sldKnob[e_LP2].setEnabled(tB);
        sldKnob[e_Bal2].setEnabled(tB);
        sldKnob[e_Mode2].setEnabled(tB);

        return;
    }

    //R1.00 Select a KNOB setting.
    if (slider == &jsP1_KnobStyle)
    {
        labHelp.setText("Select a knob style.", juce::dontSendNotification);
        otherLookAndFeel.MakoSliderKnobStyle = float(jsP1_KnobStyle.getValue());
        repaint();
        return;
    }

    return;
}

