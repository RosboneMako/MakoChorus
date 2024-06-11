/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

struct t_KnobCoors {
    float x;
    float y;
    float sizex;
    float sizey;    
};

//R1.00 Create a new LnF class based on Juces LnF class.
class MakoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //R1.00 Let the user select a knob style.
    int MakoSliderKnobStyle = 3;
    float Kpts[32];
    juce::Path pathKnob;

    MakoLookAndFeel()
    {
        //R1.00 Define the Path points to make a knob (Style 3).
        Kpts[0] = -2.65325243300477f;
        Kpts[1] = 8.60001462363607f;
        Kpts[2] = 0.0f;
        Kpts[3] = 10.0f;
        Kpts[4] = 2.65277678639377f;
        Kpts[5] = 8.60016135439157f;
        Kpts[6] = 7.81826556234706f;
        Kpts[7] = 6.23495979109873f;
        Kpts[8] = 8.3778301945593f;
        Kpts[9] = 3.28815468479365f;
        Kpts[10] = 9.74931428347318f;
        Kpts[11] = -2.22505528067641f;
        Kpts[12] = 7.79431009355225f;
        Kpts[13] = -4.4998589050713f;
        Kpts[14] = 4.3390509473009f;
        Kpts[15] = -9.00958583269659f;
        Kpts[16] = 1.34161181197136f;
        Kpts[17] = -8.89944255254108f;
        Kpts[18] = -4.33855264588318f;
        Kpts[19] = -9.00982579958681f;
        Kpts[20] = -6.12133095297134f;
        Kpts[21] = -6.59767439058605f;
        Kpts[22] = -9.74919120703023f;
        Kpts[23] = -2.22559448434896f;
        Kpts[24] = -8.97486228392824f;
        Kpts[25] = .672195644527914f;
        Kpts[26] = -7.81861038843018f;
        Kpts[27] = 6.23452737534543f;
        Kpts[28] = -5.07025014121689f;
        Kpts[29] = 7.4358969536627f;
        Kpts[30] = -2.65325243300477f;
        Kpts[31] = 8.60001462363607f;

        //R1.00 Create the actual PATH for our KNOB style 3.
        pathKnob.startNewSubPath(Kpts[0], Kpts[1]);
        for (int t = 0; t < 32; t += 2)
        {
            pathKnob.lineTo(Kpts[t], Kpts[t + 1]);
        }
        pathKnob.closeSubPath();

        //R1.00 Recreate our points with smoothed corners.
        pathKnob = pathKnob.createPathWithRoundedCorners(4.0f);
    }
    
    //R1.00 Override the Juce SLIDER drawing function so our code gets called instead of Juces code.
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& sld) override
    {
        //R1.00 Most of these are from JUCE demo code. Could be reduced if not used.
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        //R1.00 Mako Var defs.
        float sinA;
        float cosA;
        juce::ColourGradient ColGrad;
        int col;
        
        //R1.00 Defs here for future use and to make the code easier to read.
        bool DrawKnob = true;         
        bool DrawKnobEdge = true;
        bool DrawKnobNotches = true;
        

        if (DrawKnob)
        {
            //1.00 Draw the KNOB face.
            ColGrad = juce::ColourGradient(juce::Colour(0xFFF0F0E0), 0.0f, y, juce::Colour(0xFFA0A0A0), 0.0f, y + height, false);
            g.setGradientFill(ColGrad);
            g.fillEllipse(rx, ry, rw, rw);
        }       

        if (DrawKnobEdge)
        {
            //R1.00 Draw shading around knob face.
            ColGrad = juce::ColourGradient(juce::Colour(0xFF404040), 0.0f, y, juce::Colour(0xFF808080), 0.0f, y + height, false);
            g.setGradientFill(ColGrad);
            g.drawEllipse(rx - 1.0f, ry - 1.0f, rw + 2.0f, rw + 2.0f, 2.0f);

            ColGrad = juce::ColourGradient(juce::Colour(0xFFFFFFFF), 0.0f, y, juce::Colour(0xFF606060), 0.0f, y + height, false);
            g.setGradientFill(ColGrad);
            g.drawEllipse(rx, ry, rw, rw, 2.0f);
        }        
        
        //R1.00 Draw guitar style knob.
        if (MakoSliderKnobStyle == 3)
        {
            //R1.00 Copy our predefined KNOB PATH, scale it, and then transform it to the centre position.
            juce::Path p = pathKnob;
            p.applyTransform(juce::AffineTransform::scale(radius/11.0f));
            p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

            ColGrad = juce::ColourGradient(juce::Colour(0xFFFFFFFF), 0.0f, y, juce::Colour(0xFF404040), 0.0f, y + height, false);
            g.setGradientFill(ColGrad);
            g.strokePath(p, juce::PathStrokeType(2.0f));
            //g.fillPath(p);   //R1.00 Code to FILL a path.
        }

        //R1.00 Dont draw anymore objects if the control is disabled.
        if (sld.isEnabled() == false) return;

        if ((DrawKnobNotches) && (MakoSliderKnobStyle != 3))
        {
            //R1.00 Knob notches.
            ColGrad = juce::ColourGradient(juce::Colour(0xFFE0E0E0), 0.0f, y, juce::Colour(0xFF404040), 0.0f, y + height, false);
            g.setGradientFill(ColGrad);
            for (float ang = .7854f; ang < 6.0f; ang += .7854f)
            {
                sinA = std::sinf(ang + angle);
                cosA = std::cosf(ang + angle);
                g.drawLine(centreX + (sinA * radius * .9f), centreY - (cosA * radius * .9f), centreX + sinA * radius, centreY - cosA * radius, 2.0f);
            }
        }

        //R1.00 Draw finger adjust dent/indicator.
        sinA = std::sinf(angle);
        cosA = std::cosf(angle);
        auto dentSize = 12.0f;
        auto dentSize2 = 6.0f;
        auto dentX = centreX + (sinA * radius * .7f) - dentSize2;
        auto dentY = centreY - (cosA * radius * .7f) - dentSize2;

        switch (MakoSliderKnobStyle)
            {
            case 0: //R1.00 Grey shaded indent.
                ColGrad = juce::ColourGradient(juce::Colour(0xFF000000), 0.0f, dentY - dentSize2, juce::Colour(0xFFFFFFEF), 0.0f, dentY + dentSize2, false);
                g.setGradientFill(ColGrad);
                g.fillEllipse(dentX, dentY, dentSize, dentSize);
                break;
            case 1: //R1.00 Color adjusted indent.
                col = sliderPos * 255;
                g.setColour(juce::Colour(0, col, 255));
                g.fillEllipse(dentX, dentY, dentSize, dentSize);
                break;
            case 2: //R1.00 Color adjusted pointer.
                col = sliderPos * 255;
                g.setColour(juce::Colour(0, col, 255));
                g.drawLine(centreX + sinA * radius * .5f, centreY - cosA * radius * .5f, centreX + sinA * radius, centreY - cosA * radius, 4.0f);
                break;
            case 3:
                //R1.00 JUCE version of KNOB position indicator (pointer).
                //R1.00 This method may be faster and is better for drawing weird shapes.
                juce::Path p;
                auto pointerLength = radius * 0.23f;
                auto pointerThickness = 6.0f;
                p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
                p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
                
                // pointer
                g.setColour(juce::Colours::darkgrey);
                g.fillPath(p);
                
                break;
        }        
    }
};


//R1.00 Add SLIDER listener. BUTTON or TIMER listeners also go here if needed. Must add ValueChanged overrides!
class MakoBiteAudioProcessorEditor  : public juce::AudioProcessorEditor , public juce::Slider::Listener //, public juce::Button::Listener , public juce::Timer
{
public:
    MakoBiteAudioProcessorEditor (MakoBiteAudioProcessor&);
    ~MakoBiteAudioProcessorEditor() override;

    //R1.00 OUR override functions.
    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;    

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MakoBiteAudioProcessor& audioProcessor;

    //R1.00 Define the Look and Feel class we created.
    MakoLookAndFeel otherLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MakoBiteAudioProcessorEditor)

    juce::ComboBox cbPreset;
    void cbPresetChanged();
    void cbPreset_UpdateSelection(int button, int idx, int editmode);
    void Preset01();
    void Preset02();
    void Preset03();
    void Preset04();
    void Preset05();
    
    void GUI_Init_Large_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix);
    void GUI_Init_Small_Slider(juce::Slider* slider, float Val, float Vmin, float Vmax, float Vinterval, juce::String Suffix);
    
    //R1.00 Define our UI Juce Slider controls.
    int Knob_Cnt = 0;
    juce::Slider sldKnob[20];
    juce::Slider jsP1_Mono;
    juce::Slider jsP1_Chor2Enable;       
    int KnobStyle = 0;
    juce::Slider jsP1_KnobStyle;

    //R1.00 Define the coords and text for our knobs. Not JUCE related. 
    t_KnobCoors Knob_Pos[20] = {};
    juce::String Knob_Name[20] = {};
    void KNOB_DefinePosition(int t, float x, float y, float sizex, float sizey, juce::String name);

    //R1.00 Add some context sensitive help text. 
    int ctrlHelp = 0;
    int ctrlHelpLast = 0;
    juce::Label labHelp;
    juce::String HelpString[20] =
    {
        "Adjust the volume for this effect.",
        "Adjust start of the chorus delay buffer in mS. Stop-Start = Depth.",
        "Adjust end of the chorus delay buffer in mS. Stop-Start = Depth.",
        "Adjust how fast to oscillate between start/stop indexes.",
        "Adjust the mix between clean and effect signals.",
        "Adjust start of the chorus delay buffer in mS. Stop-Start = Depth.",
        "Adjust end of the chorus delay buffer in mS. Stop-Start = Depth.",
        "Adjust how fast to oscillate between start/stop indexes.",
        "Adjust the mix between clean and effect signals.",
        "Reduce high frequency signal in the chorus effect.",
        "Adjust Chorus left-right balance.",
        "Reduce high frequency signal in the chorus effect.",
        "Adjust Chorus left-right balance.",
        "Mode type: (0)Sine (1)Triangle (2)Exp (3)Rand (4)Spleen (5)Colon (6)Duane.",
        "Mode type: (0)Sine (1)Triangle (2)Exp (3)Rand (4)Spleen (5)Colon (6)Duane.",
    };

    const int e_Gain1 = audioProcessor.e_Gain1;
    const int e_Start1 = audioProcessor.e_Start1;
    const int e_Stop1 = audioProcessor.e_Stop1;
    const int e_Rate1 = audioProcessor.e_Rate1;
    const int e_Mix1 = audioProcessor.e_Mix1;
    const int e_Start2 = audioProcessor.e_Start2;
    const int e_Stop2 = audioProcessor.e_Stop2;
    const int e_Rate2 = audioProcessor.e_Rate2;
    const int e_Mix2 = audioProcessor.e_Mix2;
    const int e_LP1 = audioProcessor.e_LP1;
    const int e_Bal1 = audioProcessor.e_Bal1;
    const int e_LP2 = audioProcessor.e_LP2;
    const int e_Bal2 = audioProcessor.e_Bal2;
    const int e_Mode1 = audioProcessor.e_Mode1;
    const int e_Mode2 = audioProcessor.e_Mode2;
        

public:
    
    //R1.00 Define our SLIDER attachment variables.
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt[20];
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_Mono;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> ParAtt_Chor2Enable;
    

};
