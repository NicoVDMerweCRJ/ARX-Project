// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.
#pragma once

#include "Engine.h"
#include "Modules/ModuleManager.h"

#ifndef __JavascriptBean
#define __JavascriptBean
#include "JavascriptBean.h"
#endif

#ifndef __HTMLMenu
#define __HTMLMenu
#include "HTMLMenu.h"
#endif

/**
* The public interface to this module
*/
class IHTMLMenuMain : public IModuleInterface
{

public:

	/**
	* Singleton-like access to this module's interface.  This is just for convenience!
	* Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	*
	* @return Returns singleton instance, loading the module on demand if needed
	*/
	static inline IHTMLMenuMain& Get()
	{
		return FModuleManager::LoadModuleChecked<IHTMLMenuMain>("HTMLMenuMain");
	}

	/**
	* Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	*
	* @return True if the module is loaded and ready to use
	*/
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("HTMLMenuMain");
	}
};
