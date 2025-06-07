#include "DSPProcessing/Gain.h"

namespace DSPProcessing
{
	void FGain::Init(const float InSampleRate)
	{
		constexpr float SmoothingTimeInMs = 21.33f;
		GainParamSmoother.Init(SmoothingTimeInMs, InSampleRate);
	}

	void FGain::SetGain(const float InGain)
	{
		GainParamSmoother.SetNewParamValue(InGain);
	}

	void FGain::ProcessAudioBuffer(const float* InBuffer, float* OutBuffer, const int32 InNumSamples)
	{
		//TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FGain::ProcessAudioBuffer"))

		// Skip processing if Gain == 0
		if (GainParamSmoother.GetValue() == 0.0f)
		{
			FMemory::Memzero(OutBuffer, sizeof(float) * InNumSamples);
			return;
		}

		// Skip processing if Gain == 1
		if (GainParamSmoother.GetValue() == 1.0f)
		{
			FMemory::Memcpy(OutBuffer, InBuffer, sizeof(float) * InNumSamples);
			return;
		}

		ProcessGain(InBuffer, OutBuffer, InNumSamples);
	}

	void FGain::ProcessGain(const float* InBuffer, float* OutBuffer, const int32 InNumSamples)
	{
		// Sequential version
		//for (int32 i = 0; i < NumSamples; ++i)
		//{
		//	  OutBuffer[i] = GainParamSmoother.SmoothedParamValue() * InBuffer[i];
		//}

		// Vectorized version
		for (int32 i = 0; i < InNumSamples; i += 4)
		{
			const float CurrentGain = GainParamSmoother.GetValue();

			const VectorRegister VGain = VectorLoadFloat1(&CurrentGain);

			const VectorRegister In	 = VectorLoadAligned(&InBuffer[i]);
			const VectorRegister Out = VectorMultiply(VGain, In);

			VectorStoreAligned(Out, &OutBuffer[i]);
		}
	}
}
