// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

#include "WebBrowserAssetManagerHTMLMenu.h"

#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
#include "Materials/Material.h"
#include "Runtime/WebBrowserTexture/Public/WebBrowserTexture.h"
#endif

/////////////////////////////////////////////////////
// WebBrowserAssetManagerHTMLMenu

UWebBrowserAssetManagerHTMLMenu::UWebBrowserAssetManagerHTMLMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) ,
	DefaultMaterial(FString(TEXT("/HTMLMenuMain/WebTexture_M.WebTexture_M"))) ,
	DefaultTranslucentMaterial(FString(TEXT("/HTMLMenuMain/WebTexture_TM.WebTexture_M")))
{
#if WITH_EDITOR || PLATFORM_ANDROID || PLATFORM_IOS
	// Add a hard reference to UWebBrowserTexture, without this the WebBrowserTexture DLL never gets loaded on Windows.
	UWebBrowserTexture::StaticClass();

#endif
};

void UWebBrowserAssetManagerHTMLMenu::LoadDefaultMaterials()
{
	DefaultMaterial.LoadSynchronous();
	DefaultTranslucentMaterial.LoadSynchronous();
}

UMaterial* UWebBrowserAssetManagerHTMLMenu::GetDefaultMaterial()
{
	return DefaultMaterial.Get();
}

UMaterial* UWebBrowserAssetManagerHTMLMenu::GetDefaultTranslucentMaterial()
{
	return DefaultTranslucentMaterial.Get();
}
