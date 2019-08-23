// Copyright 2019 blurryroots interactive, Sven Freiberg. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectionistLog.h"

#ifdef IS_DEV_MODULE
// Nothing todo here
#else
#include "Modules/ModuleManager.h"

class FProjectionistModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
#endif