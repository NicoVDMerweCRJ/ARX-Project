// Copyright 2019 blurryroots interactive, Sven Freiberg. All Rights Reserved.

#include "Projectionist.h"

#ifdef IS_DEV_MODULE
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Projectionist, "Projectionist");
#else

#define LOCTEXT_NAMESPACE "FProjectionistModule"

void FProjectionistModule::StartupModule()
{
	//
}

void FProjectionistModule::ShutdownModule()
{
	//
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProjectionistModule, Projectionist)
#endif