#include "SubmixEffects/SubmixEffectGain.h"


//------------------------------------------------------------------------------------
// FSubmixEffectGain
//------------------------------------------------------------------------------------
void FSubmixEffectGain::Init(const FSoundEffectSubmixInitData& InitData)
{
	GainDSPProcessor.Init(InitData.SampleRate);
}

void FSubmixEffectGain::OnPresetChanged()
{
	GET_EFFECT_SETTINGS(SubmixEffectGain);

	GainDSPProcessor.SetGain(Settings.Gain);
}

void FSubmixEffectGain::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
	//TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FSubmixEffectGain::OnProcessAudio"))

	const float* InAudioBuffer = InData.AudioBuffer->GetData();
	float* OutAudioBuffer      = OutData.AudioBuffer->GetData();

	const int32 NumChannels = InData.NumChannels;
	const int32 NumSamples	= InData.NumFrames * NumChannels;

	GainDSPProcessor.ProcessAudioBuffer(InAudioBuffer, OutAudioBuffer, NumSamples);
}


//------------------------------------------------------------------------------------
// USubmixEffectGainPreset
//------------------------------------------------------------------------------------
void USubmixEffectGainPreset::SetSettings(const FSubmixEffectGainSettings& InSettings)
{
	UpdateSettings(InSettings);
}
