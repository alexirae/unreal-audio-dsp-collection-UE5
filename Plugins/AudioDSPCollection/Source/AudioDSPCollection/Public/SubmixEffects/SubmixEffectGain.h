#pragma once

#include "DSPProcessing/Gain.h"
#include "Sound/SoundEffectSubmix.h"

#include "SubmixEffectGain.generated.h"


//////////////////////////////////////////////////////////////////////////////////////

class AUDIODSPCOLLECTION_API FSubmixEffectGain : public FSoundEffectSubmix
{
public:
	virtual ~FSubmixEffectGain() = default;

	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSubmixInitData& InitData) override;

	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio thread.
	virtual void OnProcessAudio(const FSoundEffectSubmixInputData& InData, FSoundEffectSubmixOutputData& OutData) override;

protected:
	DSPProcessing::FGain GainDSPProcessor;
};

//////////////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct AUDIODSPCOLLECTION_API FSubmixEffectGainSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", meta = (ClampMin = "-4.0", ClampMax = "4.0", UIMin = "-4.0", UIMax = "4.0"))
	float Gain = 1.0f;
};

//////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class AUDIODSPCOLLECTION_API USubmixEffectGainPreset : public USoundEffectSubmixPreset
{
	GENERATED_BODY()

public:
	EFFECT_PRESET_METHODS(SubmixEffectGain)

	UFUNCTION(BlueprintCallable, Category = "Audio|Effects|Gain")
	void SetSettings(const FSubmixEffectGainSettings& InSettings);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubmixEffect|Preset", Meta = (ShowOnlyInnerProperties))
	FSubmixEffectGainSettings Settings;
};

//////////////////////////////////////////////////////////////////////////////////////
