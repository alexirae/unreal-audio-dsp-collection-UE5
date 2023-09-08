#include "SubmixEffects/SubmixEffectSaturation.h"


namespace
{
	DSPProcessing::ESaturationType SubmixEffectSaturationTypeToSaturationType(ESubmixEffectSaturationType SubmixEffectSaturationType)
	{
		switch (SubmixEffectSaturationType)
		{
			default:
			case ESubmixEffectSaturationType::Tape:
				return DSPProcessing::ESaturationType::Tape;
			case ESubmixEffectSaturationType::Tape2:
				return DSPProcessing::ESaturationType::Tape2;
			case ESubmixEffectSaturationType::Overdrive:
				return DSPProcessing::ESaturationType::Overdrive;
			case ESubmixEffectSaturationType::Tube:
				return DSPProcessing::ESaturationType::Tube;
			case ESubmixEffectSaturationType::Tube2:
				return DSPProcessing::ESaturationType::Tube2;
			case ESubmixEffectSaturationType::Distortion:
				return DSPProcessing::ESaturationType::Distortion;
			case ESubmixEffectSaturationType::Metal:
				return DSPProcessing::ESaturationType::Metal;
			case ESubmixEffectSaturationType::Fuzz:
				return DSPProcessing::ESaturationType::Fuzz;
			case ESubmixEffectSaturationType::HardClip:
				return DSPProcessing::ESaturationType::HardClip;
			case ESubmixEffectSaturationType::Foldback:
				return DSPProcessing::ESaturationType::Foldback;
			case ESubmixEffectSaturationType::HalfWaveRectifier:
				return DSPProcessing::ESaturationType::HalfWaveRectifier;
			case ESubmixEffectSaturationType::FullWaveRectifier:
				return DSPProcessing::ESaturationType::FullWaveRectifier;
		}
	}
}


//------------------------------------------------------------------------------------
// FSubmixEffectSaturation
//------------------------------------------------------------------------------------
void FSubmixEffectSaturation::Init(const FSoundEffectSubmixInitData& InitData)
{
	SaturationDSPProcessor.Init();

	const float SampleRate            = InitData.SampleRate;
	constexpr float SmoothingTimeInMs = 21.33f;

	SaturationDSPProcessor.InitGainParam(SmoothingTimeInMs, SampleRate);
	SaturationDSPProcessor.InitBiasParam(SmoothingTimeInMs, SampleRate);
	SaturationDSPProcessor.InitMixParam(SmoothingTimeInMs, SampleRate);
	SaturationDSPProcessor.InitOutLevelParam(SmoothingTimeInMs, SampleRate);
}

void FSubmixEffectSaturation::OnPresetChanged()
{
	GET_EFFECT_SETTINGS(SubmixEffectSaturation);

	SaturationDSPProcessor.SetSaturationType(SubmixEffectSaturationTypeToSaturationType(Settings.SaturationType));
	SaturationDSPProcessor.SetGain(Settings.Gain);
	SaturationDSPProcessor.SetBias(Settings.Bias);
	SaturationDSPProcessor.SetMix(Settings.Mix);
	SaturationDSPProcessor.SetOutLevelDb(Settings.OutLevelDb);
}

void FSubmixEffectSaturation::OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData)
{
	const float* InAudioBuffer = InData.AudioBuffer->GetData();
	float* OutAudioBuffer      = OutData.AudioBuffer->GetData();

	const int32 NumChannels = InData.NumChannels;
	const int32 NumSamples  = InData.NumFrames * NumChannels;

	SaturationDSPProcessor.ProcessAudioBuffer(InAudioBuffer, OutAudioBuffer, NumSamples);
}


//------------------------------------------------------------------------------------
// USubmixEffectSaturationPreset
//------------------------------------------------------------------------------------
void USubmixEffectSaturationPreset::SetSettings(const FSubmixEffectSaturationSettings& InSettings)
{
	UpdateSettings(InSettings);
}
