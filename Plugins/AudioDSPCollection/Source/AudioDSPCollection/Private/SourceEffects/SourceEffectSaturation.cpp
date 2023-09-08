#include "SourceEffects/SourceEffectSaturation.h"


namespace
{
	DSPProcessing::ESaturationType SourceEffectSaturationTypeToSaturationType(ESourceEffectSaturationType SourceEffectSaturationType)
	{
		switch (SourceEffectSaturationType)
		{
			default:
			case ESourceEffectSaturationType::Tape:
				return DSPProcessing::ESaturationType::Tape;
			case ESourceEffectSaturationType::Tape2:
				return DSPProcessing::ESaturationType::Tape2;
			case ESourceEffectSaturationType::Overdrive:
				return DSPProcessing::ESaturationType::Overdrive;
			case ESourceEffectSaturationType::Tube:
				return DSPProcessing::ESaturationType::Tube;
			case ESourceEffectSaturationType::Tube2:
				return DSPProcessing::ESaturationType::Tube2;
			case ESourceEffectSaturationType::Distortion:
				return DSPProcessing::ESaturationType::Distortion;
			case ESourceEffectSaturationType::Metal:
				return DSPProcessing::ESaturationType::Metal;
			case ESourceEffectSaturationType::Fuzz:
				return DSPProcessing::ESaturationType::Fuzz;
			case ESourceEffectSaturationType::HardClip:
				return DSPProcessing::ESaturationType::HardClip;
			case ESourceEffectSaturationType::Foldback:
				return DSPProcessing::ESaturationType::Foldback;
			case ESourceEffectSaturationType::HalfWaveRectifier:
				return DSPProcessing::ESaturationType::HalfWaveRectifier;
			case ESourceEffectSaturationType::FullWaveRectifier:
				return DSPProcessing::ESaturationType::FullWaveRectifier;
		}
	}
}


//------------------------------------------------------------------------------------
// FSourceEffectSaturation
//------------------------------------------------------------------------------------
void FSourceEffectSaturation::Init(const FSoundEffectSourceInitData& InitData)
{
	bIsActive   = true;
	NumChannels = InitData.NumSourceChannels;

	SaturationDSPProcessor.Init();

	const float SampleRate            = InitData.SampleRate;
	constexpr float SmoothingTimeInMs = 21.33f;

	SaturationDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
	SaturationDSPProcessor.InitBiasParam(SmoothingTimeInMs, SampleRate);
	SaturationDSPProcessor.InitMixParam(SmoothingTimeInMs, SampleRate);
	SaturationDSPProcessor.InitOutLevelParam(SmoothingTimeInMs, SampleRate);
}

void FSourceEffectSaturation::OnPresetChanged()
{
	GET_EFFECT_SETTINGS(SourceEffectSaturation);

	SaturationDSPProcessor.SetSaturationType(SourceEffectSaturationTypeToSaturationType(Settings.SaturationType));
	SaturationDSPProcessor.SetGain(Settings.Gain);
	SaturationDSPProcessor.SetBias(Settings.Bias);
	SaturationDSPProcessor.SetMix(Settings.Mix);
	SaturationDSPProcessor.SetOutLevelDb(Settings.OutLevelDb);
}

void FSourceEffectSaturation::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	const float* InAudioBuffer = InData.InputSourceEffectBufferPtr;
	float* OutAudioBuffer      = OutAudioBufferData;

	const int32 NumSamples = InData.NumSamples;

	SaturationDSPProcessor.ProcessAudioBuffer(InAudioBuffer, OutAudioBuffer, NumSamples);
}


//------------------------------------------------------------------------------------
// USourceEffectSaturationPreset
//------------------------------------------------------------------------------------
void USourceEffectSaturationPreset::SetSettings(const FSourceEffectSaturationSettings& InSettings)
{
	UpdateSettings(InSettings);
}
