/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class MakoBiteAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MakoBiteAudioProcessor();
    ~MakoBiteAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //R1.00 Add a Parameters variable.
    juce::AudioProcessorValueTreeState parameters;                           
    
    //R1.00 Our public variables.
    int SettingsChanged = 0;
    int SettingsType = 0;
    float Setting[30] = {};
    float Setting_Last[30] = {};

    float Pedal_Bal1LR[2] = { 1.0f, 1.0f };
    float Pedal_Bal2LR[2] = { 1.0f, 1.0f };
    
    int Pedal_Mono = 1;
    int Pedal_Chor2Enable = 1;

    const int e_Gain1 = 0;
    const int e_Start1 = 1;
    const int e_Stop1 = 2;
    const int e_Rate1 = 3;
    const int e_Mix1 = 4;
    const int e_Start2 = 5;
    const int e_Stop2 = 6;
    const int e_Rate2 = 7;
    const int e_Mix2 = 8;
    const int e_LP1 = 9;
    const int e_Bal1 = 10;
    const int e_LP2 = 11;
    const int e_Bal2 = 12;
    const int e_Mode1 = 13;
    const int e_Mode2 = 14;


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MakoBiteAudioProcessor)
   

    int makoGetParmValue_int(juce::String Pstring);
    float makoGetParmValue_float(juce::String Pstring);

    //R1.00 Some Constants and vars.
    const float pi = 3.14159265f;
    const float pi2 = 6.2831853f;
    const float sqrt2 = 1.4142135f;
    float SampleRate = 48000.0f;

    //R1.00 CHORUS vars.
    //R1.00 Our max is 50mS. .05S * 192kHz = 9600 samples. Add some for safety.
    const int Mod_Buffer_Size = 9999;
    float Mod1_B[2][10000] = {  };
    float Mod1_B_Last[2] = {  };
    int   Mod1_B_Idx[2] = { };
    int   Mod1_B_Idx_Max = 8000;
    int   Mod1_B_Depth = 8000;
    float Mod1_B_Offset[2] = { };
    float Mod1_B_Offset_Min = 280;
    float Mod1_B_Offset_Max = 1860;
    float Mod1_Rate[2] = { };
    float Mod1_Rate_Up = 0.0f;
    float Mod1_Rate_Down = 0.0f;

    float Mod2_B[2][10000] = {  };
    float Mod2_B_Last[2] = {  };
    int   Mod2_B_Idx[2] = { };
    int   Mod2_B_Idx_Max = 8000;
    int   Mod2_B_Depth = 8000;
    float Mod2_B_Offset[2] = { };
    float Mod2_B_Offset_Min = 280;
    float Mod2_B_Offset_Max = 1860;
    float Mod2_Rate[2] = { };
    float Mod2_Rate_Up = 0.0f;
    float Mod2_Rate_Down = 0.0f;

    int SG1_Mode = 0;
    float SG1_Val[2] = { .5f, .5f };     //R1.00 Modulation value 0-1.
    float SG1_ValBig[2] = { .5f, .5f };  //R1.00 Modulation value 0-???.
    float SG1_Rate[2] = { .01f, .01f };  //R1.00 Rate used for 0-1 mod schemes.
    float SG1_RateBig[2] = { .5f, .5f }; //R1.00 Rate used for pitch type schemes.
    float SG1_Rate_Up = .01f;
    float SG1_Rate_Dn =  -.01f;

    int SG2_Mode = 0;
    float SG2_Val[2] = { .5f, .5f };     //R1.00 Modulation value 0-1.
    float SG2_ValBig[2] = { .5f, .5f };  //R1.00 Modulation value 0-???.
    float SG2_Rate[2] = { .01f, .01f };  //R1.00 Rate used for 0-1 mod schemes.
    float SG2_RateBig[2] = { .5f, .5f }; //R1.00 Rate used for pitch type schemes.
    float SG2_Rate_Up =  .01f;
    float SG2_Rate_Dn =  -.01f;


    //R1.00 OUR FILTER VARIABLES
    struct tp_coeffs {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
        float c0;
        float d0;
    };

    struct tp_filter {
        float a0;
        float a1;
        float a2;
        float b1;
        float b2;
        float c0;
        float d0;
        float xn0[2];
        float xn1[2];
        float xn2[2];
        float yn1[2];
        float yn2[2];
        float offset[2];
    };

    //R1.00 FILTERS
    float Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn);
    void Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn);
    void Filter_LP_Coeffs(float fc, tp_filter* fn);
    void Filter_HP_Coeffs(float fc, tp_filter* fn);

    //R1.00 Our filters and function def.
    tp_filter makoF_HiCut1 = {};
    tp_filter makoF_HiCut2 = {};
    
    float MakoEffect_Chorus1(float tSample, int channel);
    float MakoEffect_Chorus2(float tSample, int channel);
    void Chorus1_CalcSettings(bool ForceAll);
    void Chorus2_CalcSettings(bool ForceAll);
    void Balance_CalcSettings(bool ForceAll);
    void Filter_CalcSettings(bool ForceAll);
    int SigGen1(int channel);
    int SigGen2(int channel);

    //R1.00 Handle any paramater changes.
    void Settings_Update(bool ForceAll);

    
};
