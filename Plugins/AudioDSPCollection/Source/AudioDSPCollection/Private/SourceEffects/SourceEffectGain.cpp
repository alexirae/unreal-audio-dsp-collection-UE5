#include "SourceEffects/SourceEffectGain.h"


//------------------------------------------------------------------------------------
// FSourceEffectGain
//------------------------------------------------------------------------------------
void FSourceEffectGain::Init(const FSoundEffectSourceInitData& InitData)
{
	bIsActive   = true;
	NumChannels = InitData.NumSourceChannels;

	const float SampleRate            = InitData.SampleRate;
	constexpr float SmoothingTimeInMs = 21.33f;

	GainDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
}

void FSourceEffectGain::OnPresetChanged()
{
	GET_EFFECT_SETTINGS(SourceEffectGain);

	GainDSPProcessor.SetGain(Settings.Gain);
}

void FSourceEffectGain::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	//TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FSourceEffectGain::ProcessAudio"))

	const float* InAudioBuffer = InData.InputSourceEffectBufferPtr;
	float* OutAudioBuffer      = OutAudioBufferData;

	const int32 NumSamples = InData.NumSamples;

	GainDSPProcessor.ProcessAudioBuffer(InAudioBuffer, OutAudioBuffer, NumSamples);
}


//------------------------------------------------------------------------------------
// USourceEffectGainPreset
//------------------------------------------------------------------------------------
void USourceEffectGainPreset::SetSettings(const FSourceEffectGainSettings& InSettings)
{
	UpdateSettings(InSettings);
}
