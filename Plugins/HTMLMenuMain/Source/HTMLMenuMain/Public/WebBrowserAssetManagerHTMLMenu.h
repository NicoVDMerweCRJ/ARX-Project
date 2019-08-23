// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

#pragma once

#include "UObject/SoftObjectPtr.h"

#include "WebBrowserAssetManagerHTMLMenu.generated.h"

class UMaterial;
/**
 * 
 */
UCLASS()
class HTMLMENUMAIN_API UWebBrowserAssetManagerHTMLMenu : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void LoadDefaultMaterials();

	UMaterial* GetDefaultMaterial(); 
	UMaterial* GetDefaultTranslucentMaterial(); 

protected:
	UPROPERTY()
	TSoftObjectPtr<UMaterial> DefaultMaterial;
	TSoftObjectPtr<UMaterial> DefaultTranslucentMaterial;
};
