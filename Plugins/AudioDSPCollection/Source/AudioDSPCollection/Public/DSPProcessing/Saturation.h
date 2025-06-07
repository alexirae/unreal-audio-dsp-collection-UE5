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

		void Init(const float InSampleRate);

		void SetSaturationType(const ESaturationType InSaturationType);
		void SetGain(const float InGain);
		void SetBias(const float InBias);
		void SetMix(const float InMixAmount);
		void SetOutLevelDb(const float InOutLevelDb);

		void ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);

	private:
		// Saturation graphs https://www.desmos.com/calculator/12d0ysis1g
		FORCEINLINE void Tape(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Tape2(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Overdrive(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Tube(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Tube2(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Distortion(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Metal(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Fuzz(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void HardClip(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void Foldback(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void HalfWaveRectifier(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);
		FORCEINLINE void FullWaveRectifier(const float* InBuffer, float* OutBuffer, const int32 InNumSamples);

		ESaturationType	 SaturationType;
		ParamSmootherLPF GainParamSmoother;
		ParamSmootherLPF BiasParamSmoother;
		ParamSmootherLPF MixParamSmoother;
		ParamSmootherLPF OutLevelParamSmoother;

		// Function pointer that points to the selected saturation type
		void (FSaturation::*SelectedSaturationTypePtr)(const float* /*InBuffer*/, float* /*OutBuffer*/, const int32 /*InNumSamples*/);
	};
}
