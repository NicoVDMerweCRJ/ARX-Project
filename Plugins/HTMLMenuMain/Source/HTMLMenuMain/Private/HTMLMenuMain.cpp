// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

#include "HTMLMenuMain.h"
#include "HTMLMenu.h"
#include "Modules/ModuleManager.h"
#include "WebBrowserAssetManagerHTMLMenu.h"
#include "Runtime/WebBrowser/Public/WebBrowserModule.h"
#include "Runtime/WebBrowser/Public/IWebBrowserSingleton.h"
#include "Materials/Material.h"


//////////////////////////////////////////////////////////////////////////
// FWebBrowserWidgetModule

class FHTMLMenuMain : public IHTMLMenuMain
{
public:
	virtual void StartupModule() override
	{
		if (WebBrowserAssetMgr == nullptr)
		{
			WebBrowserAssetMgr = NewObject<UWebBrowserAssetManagerHTMLMenu>((UObject*)GetTransientPackage(), NAME_None, RF_Transient | RF_Public);
			WebBrowserAssetMgr->LoadDefaultMaterials();

			IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
			if (WebBrowserSingleton)
			{
				WebBrowserSingleton->SetDefaultMaterial(WebBrowserAssetMgr->GetDefaultMaterial());
				WebBrowserSingleton->SetDefaultTranslucentMaterial(WebBrowserAssetMgr->GetDefaultTranslucentMaterial());
			}
		}

	}

	virtual void ShutdownModule() override
	{
	}
private:
	UWebBrowserAssetManagerHTMLMenu* WebBrowserAssetMgr;

};

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MODULE(FHTMLMenuMain, HTMLMenuMain);
