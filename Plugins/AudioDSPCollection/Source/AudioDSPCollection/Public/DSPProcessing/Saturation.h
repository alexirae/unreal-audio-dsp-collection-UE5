#pragma once

#include "DSPProcessing/Helpers/ParamSmoother.h"

namespace DSPProcessing
{
	enum class AUDIODSPCOLLECTION_API ESaturationType : int32
	{
		Tape = 0,
		Tape2,
		Overdrive,
		Tube,
		Tube2,
		Distortion,
		Metal,
		Fuzz,
		HardClip,
		Foldback,
		HalfWaveRectifier,
		FullWaveRectifier
	};

	class AUDIODSPCOLLECTION_API FSaturation
	{
	public:
		FSaturation();
		virtual ~FSaturation();

		void Init();

		void InitGainParam(float SmoothingTimeInMs, float SampleRate);
		void InitBiasParam(float SmoothingTimeInMs, float SampleRate);
		void InitMixParam(float SmoothingTimeInMs, float SampleRate);
		void InitOutLevelParam(float SmoothingTimeInMs, float SampleRate);

		void SetSaturationType(ESaturationType InSaturationType);
		void SetGain(float InGain);
		void SetBias(float InBias);
		void SetMix(float InMixAmount);
		void SetOutLevelDb(float InOutLevelDb);

		void ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, int32 NumSamples);

	private:
		// Saturation graphs https://www.desmos.com/calculator/12d0ysis1g
		FORCEINLINE void Tape(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Tape2(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Overdrive(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Tube(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Tube2(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Distortion(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Metal(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Fuzz(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void HardClip(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void Foldback(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void HalfWaveRectifier(const float* InBuffer, float* OutBuffer, int32 NumSamples);
		FORCEINLINE void FullWaveRectifier(const float* InBuffer, float* OutBuffer, int32 NumSamples);

		ESaturationType	 SaturationType;
		ParamSmootherLPF GainParamSmoother;
		ParamSmootherLPF BiasParamSmoother;
		ParamSmootherLPF MixParamSmoother;
		ParamSmootherLPF OutLevelParamSmoother;

		// Function pointer that points to the selected saturation type
		void (FSaturation::*SelectedSaturationTypePtr)(const float* /*InBuffer*/, float* /*OutBuffer*/, int32 /*NumSamples*/);
	};
}
