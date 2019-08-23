// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

#include "JavascriptBean.h"
#include "HTMLMenu.h"
#include "HTMLMenuMain.h"

UJavascriptBean::UJavascriptBean(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	onJavascriptEventTag.AddDynamic(this, &UJavascriptBean::JavascriptEventTag);
	onJavascriptEventBlank.AddDynamic(this, &UJavascriptBean::JavascriptEventBlank);
	onJavascriptEventMap.AddDynamic(this, &UJavascriptBean::JavascriptEventMap);
	onJavascriptEventJSON.AddDynamic(this, &UJavascriptBean::JavascriptEventJSON);
}

void UJavascriptBean::JavascriptEventTag(const FString ID, const FString ClassName, const FString Value, const TArray<FString>& args) {}
void UJavascriptBean::JavascriptEventBlank(const TArray<FString>& args) {}
void UJavascriptBean::JavascriptEventMap(const FStringTMap map) {}
void UJavascriptBean::JavascriptEventJSON(const FString json) {}

TMap<FString, FString> UJavascriptBean::toStringMap(FStringTMap map){
	return map.stringMap;
}

void UJavascriptBean::setBrowser(UHTMLMenu * browserP){
	browser = browserP;
}


