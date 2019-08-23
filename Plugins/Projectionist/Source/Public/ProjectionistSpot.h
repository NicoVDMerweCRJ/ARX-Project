// Copyright 2019 blurryroots interactive, Sven Freiberg. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Light.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SpotLightComponent.h"
#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif

#include "ProjectionistSpot.generated.h"

#define MATERIAL_PARAM_ASPECT TEXT("Aspect Ratio")
#define MATERIAL_PARAM_MEDIA TEXT("Media Texture")

class UMediaTexture;

/**
 * Spot light actor, supporting rgb colored movie projection via light functions.
 */
UCLASS(ClassGroup=(Lights, SpotLights), meta=(ChildCanTick))
class PROJECTIONIST_API AProjectionistSpot : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	/** Red component light function material used for movie projection. */
	UPROPERTY(EditAnywhere, Category = "Light", meta = (DisplayName = "Projection Material (Red)", AllowedClasses = "MaterialInstance"))
	FStringAssetReference ProjectionMaterialRed;

	/** Green component light function material used for movie projection. */
	UPROPERTY(EditAnywhere, Category = "Light", meta = (DisplayName = "Projection Material (Green)", AllowedClasses = "MaterialInstance"))
	FStringAssetReference ProjectionMaterialGreen;

	/** Blue component light function material used for movie projection. */
	UPROPERTY(EditAnywhere, Category = "Light", meta = (DisplayName = "Projection Material (Blue)", AllowedClasses = "MaterialInstance"))
	FStringAssetReference ProjectionMaterialBlue;

	UPROPERTY(Category = Light, VisibleAnywhere, meta = (ExposeFunctionCategories = "Light,Rendering,Rendering|Components|Light", AllowPrivateAccess = "true"))
	USpotLightComponent* RedLight;

	UPROPERTY()
	USpotLightComponent* GreenLight;

	UPROPERTY()
	USpotLightComponent* BlueLight;
	
	UPROPERTY(EditAnywhere, Category = "Light", meta = (DisplayName = "Media Texture (Override)", AllowedClasses = "MediaTexture", ToolTip="Overrides the associated media texture of the rgb spot's specified projection materials."))
	FStringAssetReference MediaTexture_Override;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UArrowComponent* ArrowComponent;
#endif

public:
	/** Aspect Ratio used for the projected movie. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Light")
	float AspectRatio = 1.6666666667;

	/** Replicated enabled indicator for the light triplet */
	UPROPERTY(replicatedUsing=OnRep_bEnabled)
	uint32 bEnabled:1;


public:
	/** Sets projection materials for each rgb componet. You can also specify a media texture to be used as an override. Will be ignored if null. */
	UFUNCTION(BlueprintCallable)
	void SetProjectionMaterials(UMaterialInterface* Red, UMaterialInterface* Green, UMaterialInterface* Blue, UMediaTexture* MediaTextureOverride);

	/** Return the dynamic material instance used for the red component of the movie projection. */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetProjectionMaterial (Red)"))
	UMaterialInstanceDynamic* GetProjectionMaterialRed()
	{
		return Cast<UMaterialInstanceDynamic>(RedLight->LightFunctionMaterial);
	}

	/** Return the dynamic material instance used for the green component of the movie projection. */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetProjectionMaterial (Green)"))
	UMaterialInstanceDynamic* GetProjectionMaterialGreen()
	{
		return Cast<UMaterialInstanceDynamic>(GreenLight->LightFunctionMaterial);
	}

	/** Return the dynamic material instance used for the blue component of the movie projection. */
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetProjectionMaterial (Blue)"))
	UMaterialInstanceDynamic* GetProjectionMaterialBlue()
	{
		return Cast<UMaterialInstanceDynamic>(BlueLight->LightFunctionMaterial);
	}
	
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	/** Replication Notification Callbacks */
	UFUNCTION()
	virtual void OnRep_bEnabled();

	/** Function to change mobility type of light */
	void SetMobility(EComponentMobility::Type InMobility);

	//~ Begin AActor Interface.
	void Destroyed() override;

	virtual bool IsLevelBoundsRelevant() const override { return false; }

	#if WITH_EDITOR
	virtual void CheckForErrors() override;

	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
	#endif
	//~ End AActor Interface.

	//~ Begin UObject Interface
	virtual void PostInitializeComponents() override;

	virtual void PostLoad() override;

	#if WITH_EDITOR
	virtual void LoadedFromAnotherClass(const FName& OldClassName) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
	//~ End UObject Interface

private:
	bool AreLightsValid() const;

	void SynchronizeLightParameters();

};
