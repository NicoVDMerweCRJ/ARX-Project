// Copyright 2019 blurryroots interactive, Sven Freiberg. All Rights Reserved.

#include "ProjectionistSpot.h"
#include "ProjectionistLog.h"
#include "Engine/Light.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Logging/TokenizedMessage.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"
#include "Materials/MaterialInterface.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/MediaAssets/Public/MediaTexture.h"

AProjectionistSpot::AProjectionistSpot(const FObjectInitializer& ObjectInitializer)	
{
	// Setup red light as root component.
	RedLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Red"));
	RedLight->Mobility = EComponentMobility::Stationary;
	//RedLight->RelativeRotation = FRotator(-90, 0, 0);
	SetRootComponent(RedLight);

	// Setup green light.
	GreenLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Green"));
	GreenLight->SetupAttachment(RootComponent);
	GreenLight->Mobility = EComponentMobility::Stationary;

	// Setup blue light.
	BlueLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Blue"));
	BlueLight->SetupAttachment(RootComponent);
	BlueLight->Mobility = EComponentMobility::Stationary;

#if WITH_EDITORONLY_DATA
	// Setup arrow component if editor build.
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->Mobility = EComponentMobility::Stationary;
#endif
}

void AProjectionistSpot::OnConstruction(const FTransform& Transform)
{
	// Configure the light colors of each component spot.
	RedLight->SetLightColor(FLinearColor::Red);
	GreenLight->SetLightColor(FLinearColor::Green);
	BlueLight->SetLightColor(FLinearColor::Blue);

	// Only create dynamic material instances, if supported.
	if (RootComponent->Mobility != EComponentMobility::Static)
	{
		// Setup materials configured through blueprint.
		auto MaterialRed = Cast<UMaterialInterface>(this->ProjectionMaterialRed.TryLoad());
		auto MaterialGreen = Cast<UMaterialInterface>(this->ProjectionMaterialGreen.TryLoad());
		auto MaterialBlue = Cast<UMaterialInterface>(this->ProjectionMaterialBlue.TryLoad());
		auto OverridingMediaTexture = Cast<UMediaTexture>(this->MediaTexture_Override.TryLoad());
		SetProjectionMaterials(MaterialRed, MaterialGreen, MaterialBlue, OverridingMediaTexture);
	}
	else
	{
		// Invalidate all materials.
		RedLight->LightFunctionMaterial = nullptr;
		GreenLight->LightFunctionMaterial = nullptr;
		BlueLight->LightFunctionMaterial = nullptr;
	}

	// Synchronize parameters across all light components.
	SynchronizeLightParameters();
}

void AProjectionistSpot::PostInitializeComponents()
{
	Super::PostInitializeComponents();
#if 0
	OnConstruction(GetActorTransform());
#endif
}

// Helper function for seting up dynamic materials to control light functions.
bool CreateOrResetDynMat(UMaterialInterface* Material, USpotLightComponent* SpotComp, float AspectRatio, UMediaTexture* Override)
{
	UMaterialInstanceDynamic* DynMat = nullptr;

	// Create and set dynamic material instance and initialize with aspect ration.
	if (nullptr != Material && Material->IsValidLowLevel())
	{
		DynMat = UMaterialInstanceDynamic::Create(Material, SpotComp);
		DynMat->SetScalarParameterValue(MATERIAL_PARAM_ASPECT, AspectRatio);
		if (Override->IsValidLowLevel())
		{
			DynMat->SetTextureParameterValue(MATERIAL_PARAM_MEDIA, Override);
		}

		SpotComp->SetLightFunctionMaterial(DynMat);
	}
	// Otherwise reset light function material.
	else
	{
		SpotComp->LightFunctionMaterial = nullptr;
	}

	return nullptr != DynMat;
}

void AProjectionistSpot::SetProjectionMaterials(UMaterialInterface* Red, UMaterialInterface* Green, UMaterialInterface* Blue, UMediaTexture* MediaTextureOverride)
{
	// Setup red light.
	if (false == CreateOrResetDynMat(Red, RedLight, AspectRatio, MediaTextureOverride))
	{
		UE_LOG(LogProjectionist, Error, TEXT("Failed to setup dynamic material for red component!"));
	}

	// Setup blue light.
	if (false == CreateOrResetDynMat(Blue, BlueLight, AspectRatio, MediaTextureOverride))
	{
		UE_LOG(LogProjectionist, Error, TEXT("Failed to setup dynamic material for blue component!"));
	}

	// Setup green light.
	if (false == CreateOrResetDynMat(Green, GreenLight, AspectRatio, MediaTextureOverride))
	{
		UE_LOG(LogProjectionist, Error, TEXT("Failed to setup dynamic material for green component!"));
	}
}

// Helper macro, syncing settings for green and blue channel to the red channel.
#define SYNC_LIGHT_PARAMETER(Parameter)\
	BlueLight->Parameter = GreenLight->Parameter = RedLight->Parameter

void AProjectionistSpot::SynchronizeLightParameters()
{
	SYNC_LIGHT_PARAMETER(AttenuationRadius);
	SYNC_LIGHT_PARAMETER(Intensity);
	SYNC_LIGHT_PARAMETER(InnerConeAngle);
	SYNC_LIGHT_PARAMETER(OuterConeAngle);
	SYNC_LIGHT_PARAMETER(IntensityUnits);
	SYNC_LIGHT_PARAMETER(bUseInverseSquaredFalloff);
	SYNC_LIGHT_PARAMETER(LightFalloffExponent);
	SYNC_LIGHT_PARAMETER(SourceRadius);
	SYNC_LIGHT_PARAMETER(SoftSourceRadius);
	SYNC_LIGHT_PARAMETER(SourceLength);
	SYNC_LIGHT_PARAMETER(LightmassSettings);
	SYNC_LIGHT_PARAMETER(Temperature);
	SYNC_LIGHT_PARAMETER(MaxDrawDistance);
	SYNC_LIGHT_PARAMETER(MaxDistanceFadeRange);
	SYNC_LIGHT_PARAMETER(bUseTemperature);
#if (ENGINE_MINOR_VERSION <= 19)
	SYNC_LIGHT_PARAMETER(MinRoughness);
#endif
	SYNC_LIGHT_PARAMETER(IndirectLightingIntensity);
	SYNC_LIGHT_PARAMETER(VolumetricScatteringIntensity);
	SYNC_LIGHT_PARAMETER(CastShadows);
	SYNC_LIGHT_PARAMETER(CastDynamicShadows);
	SYNC_LIGHT_PARAMETER(CastStaticShadows);
	SYNC_LIGHT_PARAMETER(CastTranslucentShadows);
	SYNC_LIGHT_PARAMETER(bCastVolumetricShadow);
	SYNC_LIGHT_PARAMETER(ShadowResolutionScale);
	SYNC_LIGHT_PARAMETER(ShadowBias);
	SYNC_LIGHT_PARAMETER(ShadowSharpen);
	SYNC_LIGHT_PARAMETER(ContactShadowLength);
	SYNC_LIGHT_PARAMETER(CastTranslucentShadows);
	SYNC_LIGHT_PARAMETER(bCastShadowsFromCinematicObjectsOnly);
	SYNC_LIGHT_PARAMETER(bAffectsWorld);
	SYNC_LIGHT_PARAMETER(bAffectDynamicIndirectLighting);
	SYNC_LIGHT_PARAMETER(bAffectTranslucentLighting);
	SYNC_LIGHT_PARAMETER(bForceCachedShadowsForMovablePrimitives);
	SYNC_LIGHT_PARAMETER(LightingChannels);
	SYNC_LIGHT_PARAMETER(LightFunctionScale);
	SYNC_LIGHT_PARAMETER(IESTexture);
	SYNC_LIGHT_PARAMETER(bUseIESBrightness);
	SYNC_LIGHT_PARAMETER(IESBrightnessScale);
	SYNC_LIGHT_PARAMETER(LightFunctionFadeDistance);
	SYNC_LIGHT_PARAMETER(DisabledBrightness);
	SYNC_LIGHT_PARAMETER(bEnableLightShaftBloom);
	SYNC_LIGHT_PARAMETER(BloomScale);
	SYNC_LIGHT_PARAMETER(BloomThreshold);
	SYNC_LIGHT_PARAMETER(BloomTint);
	SYNC_LIGHT_PARAMETER(bUseRayTracedDistanceFieldShadows);
	SYNC_LIGHT_PARAMETER(RayStartOffsetDepthScale);
}

// Helper function for invalidating a light in a world on destroy.
void InvalidateLight(UWorld* World, ULightComponent* LightComponent)
{
	if (LightComponent)
	{
		// Mark the light as not affecting the world before updating the shadowmap channel allocation
		LightComponent->bAffectsWorld = false;

		if (World && !World->IsGameWorld())
		{
			// Force stationary light channel preview to be updated on editor delete
			LightComponent->InvalidateLightingCache();
		}
	}
}

void AProjectionistSpot::Destroyed()
{
	UWorld* World = GetWorld();

	InvalidateLight(World, RedLight);
	InvalidateLight(World, GreenLight);
	InvalidateLight(World, BlueLight);
}

void AProjectionistSpot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectionistSpot, bEnabled);
}

void AProjectionistSpot::OnRep_bEnabled()
{
	RedLight->SetVisibility(bEnabled);
	GreenLight->SetVisibility(bEnabled);
	BlueLight->SetVisibility(bEnabled);
}

// Helper function checking if all lights are properly setup.
bool AProjectionistSpot::AreLightsValid() const
{
	return nullptr != RedLight && RedLight->IsValidLowLevelFast()
		&& nullptr != GreenLight && GreenLight->IsValidLowLevelFast()
		&& nullptr != BlueLight && BlueLight->IsValidLowLevelFast()
		;
}

void AProjectionistSpot::SetMobility(EComponentMobility::Type InMobility)
{
	bool bAreLightsValid = AreLightsValid();
	if (bAreLightsValid)
	{
		RedLight->SetMobility(InMobility);
		GreenLight->SetMobility(InMobility);
		BlueLight->SetMobility(InMobility);
	}
}

void AProjectionistSpot::PostLoad()
{
	Super::PostLoad();

	// If lights are marked as static, remove light function (not supported for light builds).
	if (RootComponent->Mobility == EComponentMobility::Static)
	{
		RedLight->LightFunctionMaterial = nullptr;
		GreenLight->LightFunctionMaterial = nullptr;
		BlueLight->LightFunctionMaterial = nullptr;

		UE_LOG(LogProjectionist, Error, TEXT("ProjectionistSpot marked as static! Lightfunctions and therefore movie projection will not be supported!"));
	}

	#if WITH_EDITORONLY_DATA
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor::White;
	}
	#endif
}

#if WITH_EDITOR

#define LOCTEXT_NAMESPACE "ErrorChecking"

void AProjectionistSpot::CheckForErrors()
{
	Super::CheckForErrors();

	bool bValidLights = AreLightsValid();
	if (false == bValidLights)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(LOCTEXT("MapCheck_Message_LightComponentNull", "ProjectionistSpot actor has invalid ProjectionistSptoComponent property - please delete!")))
			->AddToken(FMapErrorToken::Create("ProjectionistSpotComponent null"))
			;

		return;
	}

	bool bIsMarkedAsStatic = RootComponent->Mobility == EComponentMobility::Static;
	if (bIsMarkedAsStatic)
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
		FMessageLog("LightingResults").Error()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(LOCTEXT("LightingResults_Message_ProjectionistUnsupportedMobilty", "ProjectionistSpot is marked as static! Light will not be contributing to static light build!")))
			;
	}
}

void AProjectionistSpot::LoadedFromAnotherClass(const FName& OldClassName)
{
	Super::LoadedFromAnotherClass(OldClassName);

	if (GetLinkerUE4Version() < VER_UE4_REMOVE_LIGHT_MOBILITY_CLASSES)
	{
		static FName SpotLightStatic_NAME(TEXT("SpotLightStatic"));
		static FName SpotLightMovable_NAME(TEXT("SpotLightMovable"));
		static FName SpotLightStationary_NAME(TEXT("SpotLightStationary"));

		check(AreLightsValid());

		if (OldClassName == SpotLightStatic_NAME)
		{
			RedLight->Mobility = EComponentMobility::Static;
			GreenLight->Mobility = EComponentMobility::Static;
			BlueLight->Mobility = EComponentMobility::Static;
		}
		else if (OldClassName == SpotLightMovable_NAME)
		{
			RedLight->Mobility = EComponentMobility::Movable;
			GreenLight->Mobility = EComponentMobility::Movable;
			BlueLight->Mobility = EComponentMobility::Movable;
		}
		else if (OldClassName == SpotLightStationary_NAME)
		{
			RedLight->Mobility = EComponentMobility::Stationary;
			GreenLight->Mobility = EComponentMobility::Stationary;
			BlueLight->Mobility = EComponentMobility::Stationary;
		}
	}
}

// Helper function for clamping spot light angles.
void ScaleAndClampConeAngles(float& PrimaryAngle, float& SecondaryAngle, FVector ModifiedScale, float MaxAngle, float ScaleMagnitude)
{
	FMath::ApplyScaleToFloat(PrimaryAngle, ModifiedScale, ScaleMagnitude);
	PrimaryAngle = FMath::Min(89.0f, PrimaryAngle);
	SecondaryAngle = FMath::Min(PrimaryAngle, SecondaryAngle);
}

void AProjectionistSpot::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 10000.0f : 100.0f);
	const float MaxAngle = 89.0f;
	const float ScaleMagnitude = 0.01f;

	if (bCtrlDown)
	{
		ScaleAndClampConeAngles(RedLight->OuterConeAngle, RedLight->InnerConeAngle, ModifiedScale, MaxAngle, ScaleMagnitude);
		ScaleAndClampConeAngles(GreenLight->OuterConeAngle, GreenLight->InnerConeAngle, ModifiedScale, MaxAngle, ScaleMagnitude);
		ScaleAndClampConeAngles(BlueLight->OuterConeAngle, BlueLight->InnerConeAngle, ModifiedScale, MaxAngle, ScaleMagnitude);
	}
	else if (bAltDown)
	{
		ScaleAndClampConeAngles(RedLight->InnerConeAngle, RedLight->OuterConeAngle, ModifiedScale, MaxAngle, ScaleMagnitude);
		ScaleAndClampConeAngles(GreenLight->InnerConeAngle, GreenLight->OuterConeAngle, ModifiedScale, MaxAngle, ScaleMagnitude);
		ScaleAndClampConeAngles(BlueLight->InnerConeAngle, BlueLight->OuterConeAngle, ModifiedScale, MaxAngle, ScaleMagnitude);
	}
	else
	{
		FMath::ApplyScaleToFloat(RedLight->AttenuationRadius, ModifiedScale);
		FMath::ApplyScaleToFloat(GreenLight->AttenuationRadius, ModifiedScale);
		FMath::ApplyScaleToFloat(BlueLight->AttenuationRadius, ModifiedScale);
	}

	PostEditChange();
}

void AProjectionistSpot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor::White;
	}

	OnConstruction(GetActorTransform());
}
#endif // WITH_EDITOR