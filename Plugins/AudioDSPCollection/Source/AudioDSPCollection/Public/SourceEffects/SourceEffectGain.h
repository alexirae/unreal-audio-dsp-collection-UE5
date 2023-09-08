#pragma once

#include "DSPProcessing/Gain.h"
#include "Sound/SoundEffectSource.h"

#include "SourceEffectGain.generated.h"


//////////////////////////////////////////////////////////////////////////////////////

class AUDIODSPCOLLECTION_API FSourceEffectGain : public FSoundEffectSource
{
public:
	virtual ~FSourceEffectGain() = default;

	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSourceInitData& InitData) override;

	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio thread.
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;

protected:
	DSPProcessing::FGain GainDSPProcessor;
	int32 NumChannels;
};

//////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct AUDIODSPCOLLECTION_API FSourceEffectGainSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SourceEffect|Preset", meta = (ClampMin = "-4.0", ClampMax = "4.0", UIMin = "-4.0", UIMax = "4.0"))
	float Gain = 1.0f;
};

//////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class AUDIODSPCOLLECTION_API USourceEffectGainPreset : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	EFFECT_PRESET_METHODS(SourceEffectGain)

	UFUNCTION(BlueprintCallable, Category = "Audio|Effects|Gain")
	void SetSettings(const FSourceEffectGainSettings& InSettings);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SourceEffect|Preset", Meta = (ShowOnlyInnerProperties))
	FSourceEffectGainSettings Settings;
};

//////////////////////////////////////////////////////////////////////////////////////
