/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MakoBiteAudioProcessor::MakoBiteAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
    ),
    
    //R1.00 Define our VALUE TREE paramters.
    parameters(*this, nullptr, "PARAMETERS", 
      {
        std::make_unique<juce::AudioParameterFloat>("gain1","Gain 1",   .0f, 2.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("start1","Start 1", .1f, 50.0f, 4.0f),
        std::make_unique<juce::AudioParameterFloat>("stop1","Stop 1",   .1f, 50.0f, 6.0f),
        std::make_unique<juce::AudioParameterFloat>("rate1","Rate 1",  .01f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("mix1","Mix 1",     .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("start2","Start 2", .1f, 50.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("stop2","Stop 2",   .1f, 50.0f, 2.0f),
        std::make_unique<juce::AudioParameterFloat>("rate2","Rate 2",  .01f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("mix2","Mix 2",     .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterInt>("lp1","Low Pass 1", 100, 8000, 8000),
        std::make_unique<juce::AudioParameterFloat>("bal1","Bal 1",      .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterInt>("lp2","Low Pass 2", 100, 8000, 8000),
        std::make_unique<juce::AudioParameterFloat>("bal2","Bal 2",      .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterInt>("mode1","Mode 1", 0, 6, 0),
        std::make_unique<juce::AudioParameterInt>("mode2","Mode 2", 0, 6, 0),
        std::make_unique<juce::AudioParameterInt>("mono","Mono", 0, 1, 1),
        std::make_unique<juce::AudioParameterInt>("chor2enable","Chorus 2 Enable", 0, 1, 1),        
      }
    )   

#endif
{   
}

MakoBiteAudioProcessor::~MakoBiteAudioProcessor()
{
}

//==============================================================================
const juce::String MakoBiteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MakoBiteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MakoBiteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MakoBiteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MakoBiteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MakoBiteAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MakoBiteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MakoBiteAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MakoBiteAudioProcessor::getProgramName (int index)
{
    return {};
}

void MakoBiteAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MakoBiteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..


    //R1.00 Get our Sample Rate for filter calculations.
    SampleRate = MakoBiteAudioProcessor::getSampleRate();
    if (SampleRate < 21000) SampleRate = 48000;
    if (192000 < SampleRate) SampleRate = 48000;

    //R1.00 MODULATION 1
    Mod1_Rate_Up = 1.0f / SampleRate;    //R1.00 RATE How fast we traverse thru the modulation indexes.
    Mod1_Rate_Down = -1.0f / SampleRate;
    Mod1_Rate[0] = Mod1_Rate_Up;         //R1.00 Starting rate.
    Mod1_Rate[1] = Mod1_Rate_Up;         //R1.00 Starting rate.
    Mod1_B_Idx[0] = 0;                   //R1.00 index into buffer.
    Mod1_B_Idx_Max = 8000;               //R1.00 Size of buffer.
    Mod1_B_Offset[0] = 1600;             //R1.00 Where our mod index starts
    Mod1_B_Offset_Min = 280;             //R1.00 Min distance our index can get from the buffer index.
    Mod1_B_Offset_Max = 1860;            //R1.00 Max distance our index can get from the buffer index.
    Mod1_B_Idx[1] = 0;
    Mod1_B_Offset[1] = 800;   
    
    Mod2_Rate_Up = 1.0f / SampleRate;    //R1.00 RATE How fast we traverse thru the modulation indexes.
    Mod2_Rate_Down = -1.0f / SampleRate;
    Mod2_Rate[0] = Mod2_Rate_Up;         //R1.00 Starting rate.
    Mod2_Rate[1] = Mod2_Rate_Up;         //R1.00 Starting rate.
    Mod2_B_Idx[0] = 0;                   //R1.00 index into buffer.
    Mod2_B_Idx_Max = 8000;               //R1.00 Size of buffer.
    Mod2_B_Offset[0] = 1600;             //R1.00 Where our mod index starts
    Mod2_B_Offset_Min = 280;             //R1.00 Min distance our index can get from the buffer index.
    Mod2_B_Offset_Max = 1860;            //R1.00 Max distance our index can get from the buffer index.
    Mod2_B_Idx[1] = 0;
    Mod2_B_Offset[1] = 800;

    //R1.00 Update things that need updating as the program is running normally.
    //R1.00 Force every setting to be calculated.
    Settings_Update(true);
    
}

void MakoBiteAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MakoBiteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void MakoBiteAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //R1.00 Our defined variables.
    float tS;  //R1.00 Temporary Sample.

    //R1.00 Handle any changes to our Parameters in the Editor. 
    //R1.00 Dont force all updates. Just change things that have changed since last check.
    if (0 < SettingsChanged) Settings_Update(false);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        //*********************************************************
        //R1.00 Process the AUDIO buffer data. Apply our effects.
        //*********************************************************
        if (Pedal_Mono && (channel == 1))
        {
            auto* channel0Data = buffer.getWritePointer(0);

            //R1.0 FORCE MONO - Put CHANNEL 0 data in CHANNEL 1.
            for (int samp = 0; samp < buffer.getNumSamples(); samp++) channelData[samp] = channel0Data[samp];
        }
        else
        {
            // ..do something to the data...
            for (int samp = 0; samp < buffer.getNumSamples(); samp++)
            {
                //R1.00 Get the current sample and put it in tS. 
                tS = buffer.getSample(channel, samp);

                //R1.00 Apply our Distortion to the sample. 
                tS = MakoEffect_Chorus1(tS, channel);
                if (Pedal_Chor2Enable) tS = MakoEffect_Chorus2(tS, channel);
             
                //R1.00 Write our modified sample back into the sample buffer.
                channelData[samp] = Setting[e_Gain1] * tS;
            }
        }
        //**************************************************

    }
}

//==============================================================================
bool MakoBiteAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MakoBiteAudioProcessor::createEditor()
{
    return new MakoBiteAudioProcessorEditor (*this);
}


//==============================================================================
void MakoBiteAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //R1.00 Save our parameters to file/DAW.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);   
}

void MakoBiteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    //R1.00 Read our parameters from file/DAW.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    //R1.00 Get our parameters from the new settings.
    Setting[e_Gain1] = makoGetParmValue_float("gain1");
    Setting[e_Start1] = makoGetParmValue_float("start1");
    Setting[e_Stop1] = makoGetParmValue_float("stop1");
    Setting[e_Rate1] = makoGetParmValue_float("rate1");
    Setting[e_Mix1] = makoGetParmValue_float("mix1");    
    Setting[e_Start2] = makoGetParmValue_float("start2");
    Setting[e_Stop2] = makoGetParmValue_float("stop2");
    Setting[e_Rate2] = makoGetParmValue_float("rate2");
    Setting[e_Mix2] = makoGetParmValue_float("mix2");
    Setting[e_LP1] = makoGetParmValue_int("lp1");
    Setting[e_Bal1] = makoGetParmValue_float("bal1");
    Setting[e_LP2] = makoGetParmValue_int("lp2");
    Setting[e_Bal2] = makoGetParmValue_float("bal2");
    Setting[e_Mode1] = makoGetParmValue_int("mode1");
    Setting[e_Mode2] = makoGetParmValue_int("mode2");
    Pedal_Mono = makoGetParmValue_int("mono");
    Pedal_Chor2Enable = makoGetParmValue_int("chor2enable");

    //R1.00 Force all settings to be updated.
    Settings_Update(true);
}

int MakoBiteAudioProcessor::makoGetParmValue_int(juce::String Pstring)
{
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return int(parm->load());
    else
        return 0;
}

float MakoBiteAudioProcessor::makoGetParmValue_float(juce::String Pstring)
{
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return float(parm->load());
    else
        return 0.0f;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakoBiteAudioProcessor();
}


float MakoBiteAudioProcessor::Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn)
{
    float tS = tSample;

    fn->xn0[channel] = tS;
    tS = fn->a0 * fn->xn0[channel] + fn->a1 * fn->xn1[channel] + fn->a2 * fn->xn2[channel] - fn->b1 * fn->yn1[channel] - fn->b2 * fn->yn2[channel];
    fn->xn2[channel] = fn->xn1[channel]; fn->xn1[channel] = fn->xn0[channel]; fn->yn2[channel] = fn->yn1[channel]; fn->yn1[channel] = tS;

    return tS;
}

void MakoBiteAudioProcessor::Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn)
{
    //R1.00 Second order parametric/peaking boost filter with constant-Q
    float K = pi2 * (Fc * .5f) / SampleRate;
    float K2 = K * K;
    float V0 = pow(10.0, Gain_dB / 20.0);

    float a = 1.0f + (V0 * K) / Q + K2;
    float b = 2.0f * (K2 - 1.0f);
    float g = 1.0f - (V0 * K) / Q + K2;
    float d = 1.0f - K / Q + K2;
    float dd = 1.0f / (1.0f + K / Q + K2);

    fn->a0 = a * dd;
    fn->a1 = b * dd;
    fn->a2 = g * dd;
    fn->b1 = b * dd;
    fn->b2 = d * dd;
    fn->c0 = 1.0f;
    fn->d0 = 0.0f;
}

void MakoBiteAudioProcessor::Filter_LP_Coeffs(float fc, tp_filter* fn)
{
    //R1.00 Second order LOW PASS filter. 
    float c = 1.0f / (tanf(pi * fc / SampleRate));
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = 2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * (1.0f - (c * c));
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

void MakoBiteAudioProcessor::Filter_HP_Coeffs(float fc, tp_filter* fn)
{
    //F1.00 Second order butterworth High Pass.
    float c = tanf(pi * fc / SampleRate);
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = -2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * ((c * c) - 1.0f);
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

float MakoBiteAudioProcessor::MakoEffect_Chorus1(float tSample, int channel)
{
    float tS = tSample;
    
    //R1.00 Exit if not even using Modulate.
    if (Setting[e_Mix1] < .001f) return tSample;

    //R1.00 Store new sample in buffer. Apply simple LOW PASS filtering.
    if (Setting[e_LP1] < 8000) tS = Filter_Calc_BiQuad(tS, channel, &makoF_HiCut1);
    Mod1_B[channel][Mod1_B_Idx[channel]] = tS;
    
    //R1.00 Calc rotating buffer position/index.
    int idx = Mod1_B_Idx[channel] - SigGen1(channel);
    if (idx < 0) idx = idx + Mod1_B_Idx_Max;

    //R1.00 Update buffer position/index. 
    Mod1_B_Idx[channel]++;
    if (Mod1_B_Idx_Max < Mod1_B_Idx[channel]) Mod1_B_Idx[channel] = 0;

    //R1.00 MIX the clean and wet sounds. Set Balance on wet signal.
    return  (tSample * (1.0f - Setting[e_Mix1])) + (Mod1_B[channel][idx] * Setting[e_Mix1]) * Pedal_Bal1LR[channel];
}


float MakoBiteAudioProcessor::MakoEffect_Chorus2(float tSample, int channel)
{
    float tS = tSample;

    //R1.00 Exit if not even using Modulate.
    if (Setting[e_Mix2] < .001f) return tSample;
    
    //R1.00 Store new sample in buffer.  Apply simple LOW PASS filtering.
    if (Setting[e_LP2] < 8000) tS = Filter_Calc_BiQuad(tS, channel, &makoF_HiCut2);
    Mod2_B[channel][Mod2_B_Idx[channel]] = tS;
    
    //R1.00 Calc rotating buffer position/index.
    int idx = Mod2_B_Idx[channel] - SigGen2(channel);
    if (idx < 0) idx = idx + Mod2_B_Idx_Max;

    //R1.00 Update buffer position/index. 
    Mod2_B_Idx[channel]++;
    if (Mod2_B_Idx_Max < Mod2_B_Idx[channel]) Mod2_B_Idx[channel] = 0;

    //R1.00 MIX the clean and wet sounds. Set Balance on wet signal.
    return  (tSample * (1.0f - Setting[e_Mix2])) + (Mod2_B[channel][idx] * Setting[e_Mix2]) * Pedal_Bal2LR[channel];
}


void MakoBiteAudioProcessor::Settings_Update(bool ForceAll)
{
    //R1.00 We do changes here so we know the vars are not in use while we change them.
    //R1.00 EDITOR sets SETTING flags and we make changes here.
    
    //R1.00 Update our Filters.
    Filter_CalcSettings(ForceAll);
    
    //R1.00 Update our BALANCE settings.
    Balance_CalcSettings(ForceAll);

    //R1.00 Update our CHORUS settings. Start,Stop,Rate.
    Chorus1_CalcSettings(ForceAll);
    Chorus2_CalcSettings(ForceAll);
    
    //R1.00 RESET our settings flags.
    SettingsChanged -= 1;
    if (SettingsChanged < 0) SettingsChanged = 0;
}

void MakoBiteAudioProcessor::Chorus1_CalcSettings(bool ForceAll)
{
    //R1.00 CHORUS effect is created by oscillating from the MIN to MAX buffer offset.
    //R1.00 RATE sets how fast to oscillate between min/max.
    float temp;
    bool ReCalc = false;

    //R1.00 Set the Start/Stop buffer indexes from our settings.
    if ((Setting[e_Start1] != Setting_Last[e_Start1]) || ForceAll)
    {
        Setting_Last[e_Start1] = Setting[e_Start1];
        ReCalc = true;
        Mod1_B_Offset_Min = (Setting[e_Start1] * .001f) * SampleRate;
        if (Mod1_B_Offset_Min < 5) Mod1_B_Offset_Min = 5;
    }
    if ((Setting[e_Stop1] != Setting_Last[e_Stop1]) || ForceAll)
    {
        Setting_Last[e_Stop1] = Setting[e_Stop1];
        ReCalc = true;
        Mod1_B_Offset_Max = (Setting[e_Stop1] * .001f) * SampleRate;
        if (Mod_Buffer_Size < Mod1_B_Offset_Max) Mod1_B_Offset_Max = Mod_Buffer_Size;
    }

    //R1.00 Validate our settings, start must be before stop.
    if (Mod1_B_Offset_Max <= Mod1_B_Offset_Min)
    {
        //R1.00 Swap the START and STOP.
        temp = Mod1_B_Offset_Max;
        Mod1_B_Offset_Max = Mod1_B_Offset_Min;
        Mod1_B_Offset_Min = temp;

        //R1.00 Dont let MIN = MAX.
        if (Mod1_B_Offset_Min == Mod1_B_Offset_Max) Mod1_B_Offset_Max += 1.0f;
    }

    //R1.00 Check if MODE has changed.
    if ((Setting[e_Mode1] != Setting_Last[e_Mode1]) || ForceAll)
    {
        Setting_Last[e_Mode1] = Setting[e_Mode1];
        ReCalc = true;
    }
    
    //R1.00 Check if RATE has changed.
    if ((Setting[e_Rate1] != Setting_Last[e_Rate1]) || ForceAll)
    {
        Setting_Last[e_Rate1] = Setting[e_Rate1];
        ReCalc = true;
    }

    //R1.00 If we changed anything, we should recalc everything else.
    if (ReCalc)
    {
        Mod1_B_Depth = Mod1_B_Offset_Max - Mod1_B_Offset_Min;
        
        SG1_ValBig[0] = Mod1_B_Offset_Min;
        SG1_ValBig[1] = Mod1_B_Offset_Min + Mod1_B_Depth / 2;
        
        SG1_Mode = Setting[e_Mode1];
        
        SG1_Rate[0] = (5.0f * Setting[e_Rate1]) / SampleRate;
        if (SG1_Mode == 1) SG1_Rate[0] *= 2.0f;
        if (SG1_Mode == 2) SG1_Rate[0] *= 10.0f;
        
        SG1_Rate_Up = SG1_Rate[0];
        SG1_Rate_Dn = -SG1_Rate[0];
        SG1_Rate[1] = SG1_Rate[0];
        SG1_Val[0] = .0f;
        SG1_Val[1] = .5f;
    }
    
}

void MakoBiteAudioProcessor::Chorus2_CalcSettings(bool ForceAll)
{
    //R1.00 CHORUS effect is created by oscillating from the MIN to MAX buffer offset.
    //R1.00 RATE sets how fast to oscillate between min/max.
    float temp;
    bool ReCalc = false;

    //R1.00 Set the Start/Stop buffer indexes from our settings.
    if ((Setting[e_Start2] != Setting_Last[e_Start2]) || ForceAll)
    {
        Setting_Last[e_Start2] = Setting[e_Start2];
        ReCalc = true;
        Mod2_B_Offset_Min = (Setting[e_Start2] * .001f) * SampleRate;
        if (Mod2_B_Offset_Min < 5) Mod2_B_Offset_Min = 5;
    }
    if ((Setting[e_Stop2] != Setting_Last[e_Stop2]) || ForceAll)
    {
        Setting_Last[e_Stop2] = Setting[e_Stop2];
        ReCalc = true;
        Mod2_B_Offset_Max = (Setting[e_Stop2] * .001f) * SampleRate;
        if (Mod_Buffer_Size < Mod2_B_Offset_Max) Mod2_B_Offset_Max = Mod_Buffer_Size;
    }

    //R1.00 Validate our settings, start must be before stop.
    if (Mod2_B_Offset_Max <= Mod2_B_Offset_Min)
    {
        //R1.00 Swap the START and STOP.
        temp = Mod2_B_Offset_Max;
        Mod2_B_Offset_Max = Mod2_B_Offset_Min;
        Mod2_B_Offset_Min = temp;

        //R1.00 Dont let MIN = MAX.
        if (Mod2_B_Offset_Min == Mod2_B_Offset_Max) Mod2_B_Offset_Max += 1.0f;
    }

    //R1.00 Check if MODE has changed.
    if ((Setting[e_Mode2] != Setting_Last[e_Mode2]) || ForceAll)
    {
        Setting_Last[e_Mode2] = Setting[e_Mode2];
        ReCalc = true;
    }

    //R1.00 Check if RATE has changed.
    if ((Setting[e_Rate2] != Setting_Last[e_Rate2]) || ForceAll)
    {
        Setting_Last[e_Rate2] = Setting[e_Rate2];
        ReCalc = true;
    }

    //R1.00 If we changed anything, we should recalc everything else.
    if (ReCalc)
    {
        Mod2_B_Depth = Mod2_B_Offset_Max - Mod2_B_Offset_Min;

        SG2_ValBig[0] = Mod2_B_Offset_Min;
        SG2_ValBig[1] = Mod2_B_Offset_Min + Mod2_B_Depth / 2;

        SG2_Mode = Setting[e_Mode2];

        SG2_Rate[0] = (5.0f * Setting[e_Rate2]) / SampleRate;
        if (SG2_Mode == 1) SG2_Rate[0] *= 2.0f;
        if (SG2_Mode == 2) SG2_Rate[0] *= 10.0f;
       
        SG2_Rate_Up = SG2_Rate[0];
        SG2_Rate_Dn = -SG2_Rate[0];
        SG2_Rate[1] = SG2_Rate[0];
        SG2_Val[0] = .0f;
        SG2_Val[1] = .5f;
    }
}

void MakoBiteAudioProcessor::Balance_CalcSettings(bool ForceAll)
{
    //R1.00 Create left/right volume settings based on balance setting.
    //R1.00 These are multiplied by our chorus effect in processing.

    //R1.00 CHORUS 1 BALANCE settings.
    if ((Setting[e_Bal1] != Setting_Last[e_Bal1]) || ForceAll)
    {
        Setting_Last[e_Bal1] = Setting[e_Bal1];
        if (Setting[e_Bal1] < .5f)
        {
            Pedal_Bal1LR[0] = 1.0f;
            Pedal_Bal1LR[1] = Setting[e_Bal1] * 2.0f;
        }
        else
        {
            Pedal_Bal1LR[0] = 1.0f - ((Setting[e_Bal1] - .5f) * 2.0f);
            Pedal_Bal1LR[1] = 1.0f;
        }
    }

    //R1.00 CHORUS 2 BALANCE settings.
    if ((Setting[e_Bal2] != Setting_Last[e_Bal2]) || ForceAll)
    {
        Setting_Last[e_Bal2] = Setting[e_Bal2];
        if (Setting[e_Bal2] < .5f)
        {
            Pedal_Bal2LR[0] = 1.0f;
            Pedal_Bal2LR[1] = Setting[e_Bal2] * 2.0f;
        }
        else
        {
            Pedal_Bal2LR[0] = 1.0f - ((Setting[e_Bal2] - .5f) * 2.0f);
            Pedal_Bal2LR[1] = 1.0f;
        }
    }
}

void MakoBiteAudioProcessor::Filter_CalcSettings(bool ForceAll)
{
    if ((Setting[e_LP1] != Setting_Last[e_LP1]) || ForceAll)
    {
        Setting_Last[e_LP1] = Setting[e_LP1];
        Filter_LP_Coeffs(Setting[e_LP1], &makoF_HiCut1);
    }
    if ((Setting[e_LP2] != Setting_Last[e_LP2]) || ForceAll)
    {
        Setting_Last[e_LP2] = Setting[e_LP2];
        Filter_LP_Coeffs(Setting[e_LP2], &makoF_HiCut2);
    }
}

int MakoBiteAudioProcessor::SigGen1(int channel)
{
    int tIndex = 0;
    float tI;
    float r;

    switch (SG1_Mode)
    {
    case 0: //R1.00 SINE WAVE
        SG1_Val[channel] += SG1_Rate[channel];
        if (1.0f < SG1_Val[channel]) SG1_Val[channel] = .0f;
        tI = .5f + (std::sinf(SG1_Val[channel] * 6.283185f) * .5f);
        tIndex = Mod1_B_Offset_Min + (float(Mod1_B_Depth) * tI);
        break;

    case 1: //R1.00 Triangle (Linear).
        SG1_Val[channel] += SG1_Rate[channel];
        if (1.0f < SG1_Val[channel])
        {
            SG1_Val[channel] = 1.0f;
            SG1_Rate[channel] = SG1_Rate_Dn;
        }
        if (SG1_Val[channel] < .001f)
        {
            SG1_Val[channel] = .001f;
            SG1_Rate[channel] = SG1_Rate_Up;
        }
        tIndex = Mod1_B_Offset_Min + (float(Mod1_B_Depth) * SG1_Val[channel]);
        break;

    case 2: //R1.00 Exponential.
        SG1_Val[channel] *= (1.0f + SG1_Rate[channel]);
        if (1.0f < SG1_Val[channel])
        {
            SG1_Val[channel] = 1.0f;
            SG1_Rate[channel] = SG1_Rate_Dn;
        }
        if (SG1_Val[channel] < .01f)
        {
            SG1_Val[channel] = .01f;
            SG1_Rate[channel] = SG1_Rate_Up;
        }
        tIndex = Mod1_B_Offset_Min + (float(Mod1_B_Depth) * SG1_Val[channel]);
        break;    

    case 3: //R1.00 Rand.
        r = ((float(rand()) / RAND_MAX) * .002f) - .001f;
        SG1_Val[channel] += (r * Setting[e_Rate1]);
        if (1.0f < SG1_Val[channel]) SG1_Val[channel] = 1.0f;
        if (SG1_Val[channel] < 0.0f) SG1_Val[channel] = 0.0f;
        tIndex = Mod1_B_Offset_Min + (float(Mod1_B_Depth) * SG1_Val[channel]);
        break;

    case 4: //R1.00 Kidney
        SG1_ValBig[channel] += SG1_RateBig[channel];
        if (Mod1_B_Offset_Max < SG1_ValBig[channel])
        {
            SG1_ValBig[channel] = Mod1_B_Offset_Max;
            SG1_RateBig[channel] = -.5f;
        }
        if (SG1_ValBig[channel] < Mod1_B_Offset_Min)
        {
            SG1_ValBig[channel] = Mod1_B_Offset_Min;
            SG1_RateBig[channel] = 1.0f;
        }
        tIndex = SG1_ValBig[channel];
        break;

    case 5: //R1.00 Colon
        SG1_ValBig[channel] += SG1_RateBig[channel];
        if (Mod1_B_Offset_Max < SG1_ValBig[channel])
        {
            SG1_ValBig[channel] = Mod1_B_Offset_Max;
            SG1_RateBig[channel] = -.5f;
        }
        if (SG1_ValBig[channel] < Mod1_B_Offset_Min)
        {
            SG1_ValBig[channel] = Mod1_B_Offset_Min;
            SG1_RateBig[channel] = .5f;
        }
        tIndex = SG1_ValBig[channel];
        break;

    case 6: //R1.00 Duane
        SG1_ValBig[channel] += SG1_RateBig[channel];
        if (Mod1_B_Offset_Max < SG1_ValBig[channel])
        {
            SG1_ValBig[channel] = Mod1_B_Offset_Max;
            SG1_RateBig[channel] = -.25f;
        }
        if (SG1_ValBig[channel] < Mod1_B_Offset_Min)
        {
            //R1.00 This will have loud pops as the buffer index resets nonlinearly.
            //SG1_ValBig[channel] = Mod1_B_Offset_Max;   

            //R1.00 Normal transition/modulation.
            SG1_ValBig[channel] = Mod1_B_Offset_Min;
            SG1_RateBig[channel] = 1.0f;
        }
        tIndex = SG1_ValBig[channel];
        break;
    }

    return tIndex;
}

int MakoBiteAudioProcessor::SigGen2(int channel)
{
    int tIndex = 0;
    float tI;
    float r;

    switch (SG2_Mode)
    {
    case 0: //R1.00 SINE WAVE
        SG2_Val[channel] += SG2_Rate[channel];
        if (1.0f < SG2_Val[channel]) SG2_Val[channel] = .0f;
        tI = .5f + (std::sinf(SG2_Val[channel] * 6.283185f) * .5f);
        tIndex = Mod2_B_Offset_Min + (float(Mod2_B_Depth) * tI);
        break;

    case 1: //R1.00 Triangle (Linear).
        SG2_Val[channel] += SG2_Rate[channel];
        if (1.0f < SG2_Val[channel])
        {
            SG2_Val[channel] = 1.0f;
            SG2_Rate[channel] = SG2_Rate_Dn;
        }
        if (SG2_Val[channel] < .001f)
        {
            SG2_Val[channel] = .001f;
            SG2_Rate[channel] = SG2_Rate_Up;
        }
        tIndex = Mod2_B_Offset_Min + (float(Mod2_B_Depth) * SG2_Val[channel]);
        break;

    case 2: //R1.00 Exponential.
        SG2_Val[channel] *= (1.0f + SG2_Rate[channel]);
        if (1.0f < SG2_Val[channel])
        {
            SG2_Val[channel] = 1.0f;
            SG2_Rate[channel] = SG2_Rate_Dn;
        }
        if (SG2_Val[channel] < .01f)
        {
            SG2_Val[channel] = .01f;
            SG2_Rate[channel] = SG2_Rate_Up;
        }
        tIndex = Mod2_B_Offset_Min + (float(Mod2_B_Depth) * SG2_Val[channel]);
        break;

    case 3: //R1.00 Rand.
        r = ((float(rand()) / RAND_MAX) * .001f) - .0005f;
        SG2_Val[channel] += (r * Setting[e_Rate2]);
        if (1.0f < SG2_Val[channel]) SG2_Val[channel] = 1.0f;
        if (SG2_Val[channel] < 0.0f) SG2_Val[channel] = 0.0f;
        tIndex = Mod2_B_Offset_Min + (float(Mod2_B_Depth) * SG2_Val[channel]);
        break;

    case 4: //R1.00 Spleen
        SG2_ValBig[channel] += SG2_RateBig[channel];
        if (Mod2_B_Offset_Max < SG2_ValBig[channel])
        {
            SG2_ValBig[channel] = Mod2_B_Offset_Max;
            SG2_RateBig[channel] = -.5f;
        }
        if (SG2_ValBig[channel] < Mod2_B_Offset_Min)
        {
            SG2_ValBig[channel] = Mod2_B_Offset_Min;
            SG2_RateBig[channel] = 1.0f;
        }
        tIndex = SG2_ValBig[channel];
        break;

    case 5: //R1.00 Colon
        SG2_ValBig[channel] += SG2_RateBig[channel];
        if (Mod2_B_Offset_Max < SG2_ValBig[channel])
        {
            SG2_ValBig[channel] = Mod2_B_Offset_Max;
            SG2_RateBig[channel] = -.5f;
        }
        if (SG2_ValBig[channel] < Mod2_B_Offset_Min)
        {
            SG2_ValBig[channel] = Mod2_B_Offset_Min;
            SG2_RateBig[channel] = .5f;
        }
        tIndex = SG2_ValBig[channel];
        break;

    case 6: //R1.00 Duane
        SG2_ValBig[channel] += SG2_RateBig[channel];
        if (Mod2_B_Offset_Max < SG2_ValBig[channel])
        {
            SG2_ValBig[channel] = Mod2_B_Offset_Max;
            SG2_RateBig[channel] = -.25f;
        }
        if (SG2_ValBig[channel] < Mod2_B_Offset_Min)
        {
            SG2_ValBig[channel] = Mod2_B_Offset_Min;
            SG2_RateBig[channel] = 1.0f;
        }
        tIndex = SG2_ValBig[channel];
        break;
    }

    return tIndex;
}
