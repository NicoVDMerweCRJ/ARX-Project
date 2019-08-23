// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

#include "HTMLMenu.h"
#include "HTMLMenuMain.h"
#include "SWebBrowser.h"
#include "Runtime/WebBrowser/Public/WebBrowserModule.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"

#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/HAL/FileManager.h"

#if WITH_EDITOR
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialFunction.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "PackageHelperFunctions.h"
#endif

#define LOCTEXT_NAMESPACE "HTMLMenuMain"

/////////////////////////////////////////////////////
// UHTMLMenu


TMap<FString, UJavascriptBean*> UHTMLMenu::javascriptBeans;

UHTMLMenu::UHTMLMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;

	onJavascriptEventMouseDown.AddDynamic(this, &UHTMLMenu::JavascriptEventMouseDown);
	onJavascriptEventMouseUp.AddDynamic(this, &UHTMLMenu::JavascriptEventMouseUp);

}




void UHTMLMenu::JavascriptEventMouseDown(const float X, const float Y, const int32 button) {}
void UHTMLMenu::JavascriptEventMouseUp(const float X, const float Y, const int32 button) {}


void UHTMLMenu::appReactivtedDelegate() {
	//if (websocketServerThread == nullptr) {
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("appReactivtedDelegate 1"));
			//if (websocketServerThread != nullptr) {
			//	websocketServerThread->stopThread();
			//	FPlatformProcess::Sleep(0.2);
			//}
	websocketServerThread = new FHtmlMenuWebsocketServerThread(this);
	//}
}

void UHTMLMenu::appEnterBackgroundDelegate() {
	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, TEXT("appEnterBackgroundDelegate"));
	//if (websocketServerThread != nullptr) {
	//	websocketServerThread->stopThread();
	//	websocketServerThread = nullptr;
	//}
}

void UHTMLMenu::setWebsocketPort(int32 port) {
	lastWebsocketPort = port;
}

int32 UHTMLMenu::getWebsocketPort() {
	return lastWebsocketPort;
}





void UHTMLMenu::registerJavascriptEvent(const FString nameOfJavascriptFunction, UJavascriptBean*& eventTarget) {
	if (!HTMLMenu.IsValid())
		return;

	if (widgetID.IsEmpty())
		widgetID = FGuid::NewGuid().ToString();

	FString lowerName = nameOfJavascriptFunction.ToLower() + widgetID;

	UE_LOG(LogTemp, Warning, TEXT("HTMLMenu registerJavascriptEvent: %s"), *lowerName);

	UPROPERTY()
		UJavascriptBean* javascriptBean = NewObject<UJavascriptBean>(UJavascriptBean::StaticClass());
	javascriptBean->AddToRoot();
	javascriptBean->setBrowser(this);
	javascriptBeans.Add(lowerName, javascriptBean);
	eventTarget = javascriptBean;

}


void UHTMLMenu::triggerUE4EventWithTag(FString javascriptFunctionsName, FString ID, FString className, FString value, FString arguments) {

	FString lowerName = javascriptFunctionsName.ToLower() + widgetID;
	if (javascriptBeans.Num() == 0 || javascriptBeans.Find(lowerName) == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("HTMLMenu Javascript Function %s is not registered."), *lowerName);
		return;
	}
	UJavascriptBean* javascriptBean = *javascriptBeans.Find(lowerName);

	//UE_LOG(LogTemp, Display, TEXT("JAVASCRIPT:%s"), *arguments);
	TArray<FString> args;
	arguments.ParseIntoArray(args, TEXT(".;_|"), true);
	AsyncTask(ENamedThreads::GameThread, [javascriptBean, ID, className, value, args]() {
		javascriptBean->onJavascriptEventTag.Broadcast(ID, className, value, args);
		});
}

void UHTMLMenu::triggerUE4EventWithBlank(FString javascriptFunctionsName, FString arguments) {

	FString lowerName = javascriptFunctionsName.ToLower() + widgetID;

	//UE_LOG(LogTemp, Warning, TEXT("HTMLMenu triggerUE4EventWithBlank: %s"), *lowerName);

	if (javascriptBeans.Num() == 0 || javascriptBeans.Find(lowerName) == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("HTMLMenu Javascript Function %s is not registered."), *lowerName);
		return;
	}
	UJavascriptBean* javascriptBean = *javascriptBeans.Find(lowerName);

	//UE_LOG(LogTemp, Display, TEXT("JAVASCRIPT:%s"), *arguments);
	TArray<FString> args;
	arguments.ParseIntoArray(args, TEXT(".;_|"), true);
	AsyncTask(ENamedThreads::GameThread, [javascriptBean, args]() {
		javascriptBean->onJavascriptEventBlank.Broadcast(args);
		});
}

void UHTMLMenu::triggerUE4EventWithMap(FString javascriptFunctionsName, FString arguments) {

	FString lowerName = javascriptFunctionsName.ToLower() + widgetID;
	if (javascriptBeans.Num() == 0 || javascriptBeans.Find(lowerName) == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("HTMLMenu Javascript Function %s is not registered."), *lowerName);
		return;
	}
	UJavascriptBean* javascriptBean = *javascriptBeans.Find(lowerName);

	FStringTMap stringTMap;
	//UE_LOG(LogTemp, Display, TEXT("JAVASCRIPT:%s"), *arguments);
	TArray<FString> args;
	arguments.ParseIntoArray(args, TEXT(".;_|"), true);
	for (int i = 0; i < args.Num(); i++) {
		FString left;
		FString right;
		args[i].Split(":", &left, &right);
		stringTMap.stringMap.Add(left, right);
	}
	AsyncTask(ENamedThreads::GameThread, [javascriptBean, stringTMap]() {
		javascriptBean->onJavascriptEventMap.Broadcast(stringTMap);
		});
}

void UHTMLMenu::triggerUE4EventWithJSON(FString javascriptFunctionsName, FString json) {

	FString lowerName = javascriptFunctionsName.ToLower() + widgetID;
	if (javascriptBeans.Num() == 0 || javascriptBeans.Find(lowerName) == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("HTMLMenu Javascript Function %s is not registered."), *lowerName);
		return;
	}
	UJavascriptBean* javascriptBean = *javascriptBeans.Find(lowerName);


	AsyncTask(ENamedThreads::GameThread, [javascriptBean, json]() {
		javascriptBean->onJavascriptEventJSON.Broadcast(json);
		});
}


void UHTMLMenu::triggerUE4EventMouseDown(float x, float y, int32 button) {

	UHTMLMenu* HTMLMenuLambda = this;

	AsyncTask(ENamedThreads::GameThread, [HTMLMenuLambda, x, y, button]() {
		HTMLMenuLambda->onJavascriptEventMouseDown.Broadcast(x, y, button);
		});
}

void UHTMLMenu::triggerUE4EventMouseUp(float x, float y, int32 button) {

	UHTMLMenu* HTMLMenuLambda = this;

	AsyncTask(ENamedThreads::GameThread, [HTMLMenuLambda, x, y, button]() {
		HTMLMenuLambda->onJavascriptEventMouseUp.Broadcast(x, y, button);
		});
}

void UHTMLMenu::javascriptConsoleCommand(FString command) {
	AsyncTask(ENamedThreads::GameThread, [command]() {
		if (GEngine && GEngine != nullptr) {
			APlayerController* PController = UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0);
			if (PController) {
				PController->ConsoleCommand(command);
			}
		}
		});
}

void UHTMLMenu::javascriptQuitGame() {
	UHTMLMenu* self = this;
	AsyncTask(ENamedThreads::GameThread, [self]() {
		if (GEngine && GEngine != nullptr) {
			APlayerController* PController = UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0);
			if (PController) {
				UKismetSystemLibrary::QuitGame(self, PController, EQuitPreference::Quit, true);
			}
		}
		});
}

void UHTMLMenu::javascriptApplyAndSaveGraphics(FString resolutionString, int32 windowMode, int32 antiAliasing, int32 shadows, float resolutionScale) {
	AsyncTask(ENamedThreads::GameThread, [resolutionString, windowMode, antiAliasing, shadows, resolutionScale]() {
		if (GEngine && GEngine != nullptr) {
			UGameUserSettings* gameSettings = GEngine->GameUserSettings;


			//RESOLUTION
			FIntPoint resolution;
			FString resX;
			FString resY;
			resolutionString.Split("x", &resX, &resY);
			resolution.X = FCString::Atoi(*resX);
			resolution.Y = FCString::Atoi(*resY);

			gameSettings->SetScreenResolution(resolution);

			//Full or Window Mode
			switch (windowMode) {
			case 0:
				gameSettings->SetFullscreenMode(EWindowMode::Type::Fullscreen);
				break;
			case 1:
				gameSettings->SetFullscreenMode(EWindowMode::Type::WindowedFullscreen);
				break;
			case 2:
				gameSettings->SetFullscreenMode(EWindowMode::Type::Windowed);
				break;
			}

			//Anti-Aliasing
			gameSettings->SetAntiAliasingQuality(antiAliasing);
			//Shadows
			gameSettings->SetShadowQuality(shadows);

			//Resolution Scale
			float rs = resolutionScale / 10;
			gameSettings->SetResolutionScaleNormalized(rs);

			gameSettings->ApplySettings(false);
			gameSettings->SaveSettings();
		}
		});
}

void UHTMLMenu::javascriptGetGraphics() {
	if (!HTMLMenu.IsValid() || HTMLMenu.Get() == nullptr) {
		return;
	}

	if (GEngine && GEngine != nullptr && GEngine != NULL) {
		UGameUserSettings* gameSettings = GEngine->GameUserSettings;
		FString res = FString::FromInt(gameSettings->GetScreenResolution().X) + "x" + FString::FromInt(gameSettings->GetScreenResolution().Y);
		FString windowMode = FString::FromInt(gameSettings->GetFullscreenMode());
		FString antiAliasing = FString::FromInt(gameSettings->GetAntiAliasingQuality());
		FString shadows = FString::FromInt(gameSettings->GetShadowQuality());

		float CurrentScaleNormalized;
		float CurrentScaleValue;
		float MinScaleValue;
		float MaxScaleValue;

		gameSettings->GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue);

		FString resolutionScale = FString::FromInt((CurrentScaleNormalized * 10));
		if (HTMLMenu.IsValid() && HTMLMenu.Get() != nullptr)
			HTMLMenu->ExecuteJavascript("UE4SetGraphics(\"" + res + "\"," + windowMode + "," + antiAliasing + "," + shadows + "," + resolutionScale + ",false);");
	}
}

void UHTMLMenu::javascriptClientTravel(FString mapOrAdress) {
	AsyncTask(ENamedThreads::GameThread, [mapOrAdress]() {
		if (GEngine && GEngine != nullptr) {
			APlayerController* PController = UGameplayStatics::GetPlayerController(GEngine->GameViewport->GetWorld(), 0);
			if (PController) {
				PController->ClientTravel(mapOrAdress, ETravelType::TRAVEL_Absolute);
			}
		}
		});
}

void UHTMLMenu::javascriptMousePosition(float x, float y) {
	htmlMouseCursorPositionX = x;
	htmlMouseCursorPositionY = y;
}

void UHTMLMenu::getHTMLMouseCursorPosition(float& x, float& y) {
	x = htmlMouseCursorPositionX;
	y = htmlMouseCursorPositionY;
}

UHTMLMenu* UHTMLMenu::generateHTML_File(FString sourceHTML_File, FString targetHTML_Dir, bool overwriteExistingFile) {
	//run only in editor
#if !WITH_EDITOR
	return this;
#endif


	if (!FPaths::FileExists(sourceHTML_File)) {
		UE_LOG(LogTemp, Error, TEXT("(56)HTML Menu: HTML File not found: %s"), *sourceHTML_File);
		return this;
	}

	//dir not file 
	if (FPaths::FileExists(targetHTML_Dir) || targetHTML_Dir.Contains(".html")) {
		targetHTML_Dir = FPaths::GetPath(targetHTML_Dir);
	}

	FString sourceHTML_FileName = FPaths::GetCleanFilename(sourceHTML_File);

	FString htmlMenuDir = FPaths::ProjectDir() + targetHTML_Dir;
	//htmlMenuDir = FPaths::ConvertRelativePathToFull(htmlMenuDir);

	//exist? then just load it. //todo: check file time stamp
	if (!overwriteExistingFile && FPaths::FileExists(htmlMenuDir + "/" + sourceHTML_FileName)) {
		return this;
	}

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!platformFile.CreateDirectoryTree(*(FPaths::GetPath(htmlMenuDir)))) {
		UE_LOG(LogTemp, Error, TEXT("(47)HTML Menu: Can't create Directory: %s"), *htmlMenuDir);
		return this;
	}


	createAndMinifyHTMLMenu(sourceHTML_File, htmlMenuDir);

	return this;

}

void UHTMLMenu::createAndMinifyHTMLMenu(FString sourceHTML_File, FString targetDir) {
	FString sourceDir = FPaths::GetPath(sourceHTML_File);


	FString content;
	bool isFileRead = FFileHelper::LoadFileToString(content, *sourceHTML_File);
	content = content.Replace(TEXT("\r"), TEXT(""));
	content = content.Replace(TEXT("\t"), TEXT(""));
	TArray<FString> lines;
	int32 lineCount = content.ParseIntoArray(lines, TEXT("\n"), true);
	content.Empty();

	for (int32 i = 0; i < lineCount; i++) {
		FString s = lines[i];
		//javascript includes
		if (s.Contains("<script") && s.Contains("</script>") && s.Contains(".js\"")) {
			s = s.Replace(TEXT("type=\"text/javascript\""), TEXT(""));
			FString left;
			FString right;
			s.Split(".js", &left, &right);
			FString js = left;
			js.Split("src=\"", &left, &right);
			FString url = sourceDir + "/" + right + ".js";

			if (!FPaths::FileExists(url)) {
				UE_LOG(LogTemp, Warning, TEXT("Can't read js file: %s"), *url);
				content.Append(s);
				continue;
			}

			FString jsContent;
			if (FFileHelper::LoadFileToString(jsContent, *url)) {
				jsContent = removeJavascriptComments(jsContent);//jsContent.Replace(TEXT("\n"), TEXT(""));
																//UE_LOG(LogTemp, Display, TEXT("%s"), *cssContent);
				content.Append("<script>" + jsContent + "</script>");
			}
		}
		else {
			//css includes
			if (s.Contains("<link") && (s.Contains(".css\"") || s.Contains(".css'")) && (s.Contains("href=\"") || s.Contains("href=\'"))) {
				s = s.Replace(TEXT("'"), TEXT("\""));
				FString left;
				FString right;
				s.Split(".css", &left, &right);
				s = left;
				s.Split("href=\"", &left, &right);
				FString url = sourceDir + "/" + right + ".css";
				if (!FPaths::FileExists(url)) {
					UE_LOG(LogTemp, Warning, TEXT("Can't read css file: %s"), *url);
					continue;
				}

				FString cssDir = FPaths::GetPath(url);
				FString cssContent;
				if (FFileHelper::LoadFileToString(cssContent, *url)) {
					cssContent = cssContent.Replace(TEXT("\n"), TEXT(""));
					cssContent = replaceCSSImages(cssContent, cssDir);
					cssContent = replaceCSSFonts(cssContent, cssDir);
					//UE_LOG(LogTemp, Display, TEXT("%s"), *cssContent);
					content.Append("<style>" + cssContent + "</style>");
				}
			}
			else {
				//without comments
				if (s.Contains("//")) {
					FString b = s.TrimStart();
					if (!b.StartsWith("//")) {
						content.Append(s);
					}
				}
				else {
					if (s.Contains("<img")) {
						s = replaceHTMLImages(s, sourceDir);
					}
					content.Append(s);
				}
			}
		}
	}

	FFileHelper::SaveStringToFile(content, *(targetDir + "/" + FPaths::GetCleanFilename(sourceHTML_File)));

}



FString UHTMLMenu::replaceCSSImages(FString content, FString cssURL) {
	FString out;

	content = content.Replace(TEXT("url(\""), TEXT("url(\"\n"));
	content = content.Replace(TEXT(".png\")"), TEXT(".png\n\")"));
	content = content.Replace(TEXT(".gif\")"), TEXT(".gif\n\")"));
	content = content.Replace(TEXT(".jpg\")"), TEXT(".jpg\n\")"));
	content = content.Replace(TEXT(".jpeg\")"), TEXT(".jpeg\n\")"));

	content = content.Replace(TEXT("url('"), TEXT("url('\n"));
	content = content.Replace(TEXT(".png')"), TEXT(".png\n')"));
	content = content.Replace(TEXT(".gif')"), TEXT(".gif\n')"));
	content = content.Replace(TEXT(".jpg')"), TEXT(".jpg\n')"));
	content = content.Replace(TEXT(".jpeg')"), TEXT(".jpeg\n')"));

	content = content.Replace(TEXT("url("), TEXT("url(\n"));
	content = content.Replace(TEXT(".png)"), TEXT(".png\n)"));
	content = content.Replace(TEXT(".gif)"), TEXT(".gif\n)"));
	content = content.Replace(TEXT(".jpg)"), TEXT(".jpg\n)"));
	content = content.Replace(TEXT(".jpeg)"), TEXT(".jpeg\n)"));

	TArray<FString> lines;
	int32 lineCount = content.ParseIntoArray(lines, TEXT("\n"), true);
	for (int32 i = 0; i < lineCount; i++) {
		FString s = lines[i];
		if (s.Contains(".png") || s.Contains(".gif") || s.Contains(".jpg") || s.Contains(".jpeg")) {
			FString image = cssURL + "/" + s;
			if (FPaths::FileExists(image)) {
				TArray<uint8> fileData;
				if (FFileHelper::LoadFileToArray(fileData, *image)) {
					FString base64Image = FBase64::Encode(fileData);

					out += "data:image/" + FPaths::GetExtension(*image, true) + ";base64," + base64Image;
					continue;
				}
			}
		}

		out += s;
	}
	return out;
}

FString UHTMLMenu::replaceCSSFonts(FString content, FString cssURL) {
	FString out;

	content = content.Replace(TEXT("url(\""), TEXT("url(\"\n"));
	content = content.Replace(TEXT(".woff2\")"), TEXT(".woff2\n\")"));
	content = content.Replace(TEXT(".woff\")"), TEXT(".woff\n\")"));
	content = content.Replace(TEXT(".ttf\")"), TEXT(".ttf\n\")"));
	content = content.Replace(TEXT(".svg\")"), TEXT(".svg\n\")"));

	content = content.Replace(TEXT("url('"), TEXT("url('\n"));
	content = content.Replace(TEXT(".woff2')"), TEXT(".woff2\n')"));
	content = content.Replace(TEXT(".woff')"), TEXT(".woff\n')"));
	content = content.Replace(TEXT(".ttf')"), TEXT(".ttf\n')"));
	content = content.Replace(TEXT(".svg')"), TEXT(".svg\n')"));

	content = content.Replace(TEXT("url("), TEXT("url(\n"));
	content = content.Replace(TEXT(".woff2)"), TEXT(".woff2\n)"));
	content = content.Replace(TEXT(".woff)"), TEXT(".woff\n)"));
	content = content.Replace(TEXT(".ttf)"), TEXT(".ttf\n)"));
	content = content.Replace(TEXT(".svg)"), TEXT(".svg\n)"));

	TArray<FString> lines;
	int32 lineCount = content.ParseIntoArray(lines, TEXT("\n"), true);
	for (int32 i = 0; i < lineCount; i++) {
		FString s = lines[i];
		if (s.Contains(".woff2") || s.Contains(".woff") || s.Contains(".ttf") || s.Contains(".svg")) {
			FString font = cssURL + "/" + s;
			if (FPaths::FileExists(font)) {
				TArray<uint8> fileData;
				if (FFileHelper::LoadFileToArray(fileData, *font)) {
					FString base64Image = FBase64::Encode(fileData);

					out += "data:application/x-font-" + FPaths::GetExtension(*font, true) + ";charset=utf-8;base64," + base64Image;
					continue;
				}
			}
		}

		out += s;
	}
	return out;
}

FString UHTMLMenu::replaceHTMLImages(FString content, FString htmlURL) {
	FString out;

	content = content.Replace(TEXT("src=\""), TEXT("src=\"\n"));
	content = content.Replace(TEXT(".png\""), TEXT(".png\n\""));
	content = content.Replace(TEXT(".gif\""), TEXT(".gif\n\""));
	content = content.Replace(TEXT(".jpg\""), TEXT(".jpg\n\""));
	content = content.Replace(TEXT(".jpeg\""), TEXT(".jpeg\n\""));

	content = content.Replace(TEXT("src='"), TEXT("src='\n"));
	content = content.Replace(TEXT(".png'"), TEXT(".png\n'"));
	content = content.Replace(TEXT(".gif'"), TEXT(".gif\n'"));
	content = content.Replace(TEXT(".jpg'"), TEXT(".jpg\n'"));
	content = content.Replace(TEXT(".jpeg'"), TEXT(".jpeg\n'"));

	TArray<FString> lines;
	int32 lineCount = content.ParseIntoArray(lines, TEXT("\n"), true);
	for (int32 i = 0; i < lineCount; i++) {
		FString s = lines[i];
		if (s.Contains(".png") || s.Contains(".gif") || s.Contains(".jpg") || s.Contains(".jpeg")) {
			FString image = htmlURL + "/" + s;
			if (FPaths::FileExists(image)) {
				TArray<uint8> fileData;
				if (FFileHelper::LoadFileToArray(fileData, *image)) {
					FString base64Image = FBase64::Encode(fileData);

					out += "data:image/" + FPaths::GetExtension(*image, true) + ";base64," + base64Image;
					continue;
				}
			}
		}

		out += s;
	}
	return out;
}

FString UHTMLMenu::removeJavascriptComments(FString content) {
	FString out;
	TArray<FString> lines;
	int32 lineCount = content.ParseIntoArray(lines, TEXT("\n"), true);
	for (int32 i = 0; i < lineCount; i++) {
		if (lines[i].Contains("//")) {
			FString b = lines[i].TrimStart();
			if (!b.StartsWith("//")) {
				out.Append(lines[i]);
			}
		}
		else {
			out.Append(lines[i]);
		}
	}
	return out;
}

UHTMLMenu* UHTMLMenu::loadHTML_File(FString htmlFile, bool removeLineBreaks) {

	if (!HTMLMenu.IsValid())
		return this;


	FString htmlMenuDir = FPaths::ProjectDir() + htmlFile;
	//clean path
	htmlMenuDir = htmlMenuDir.Replace(TEXT("\\\\"), TEXT("\\"));
	htmlMenuDir = htmlMenuDir.Replace(TEXT("\\"), TEXT("/"));

	if (!FPaths::FileExists(htmlMenuDir)) {
		UE_LOG(LogTemp, Error, TEXT("(86)File not found:%s"), *htmlMenuDir);
		return this;
	}

	FString content;
	bool isFileRead = FFileHelper::LoadFileToString(content, *htmlMenuDir);
	if (isFileRead) {
		if (removeLineBreaks) {
			content = content.Replace(TEXT("\t"), TEXT(""));
			content = content.Replace(TEXT("\r"), TEXT(""));
			content = content.Replace(TEXT("\n"), TEXT(""));
		}
		currentURL = "virtualbird.de/ue4htmlmenuplugin.html";
		LoadString(content, currentURL);
		HTMLMenu->BindUObject("uecom", this, true);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("(96)Can't read file:%s"), *htmlMenuDir);
	}

	return this;
}

void UHTMLMenu::triggerKeyAction(EHTML_JavascriptFunction function) {
	if (!HTMLMenu.IsValid()) {
		return;
	}
	switch (function) {
	case EHTML_JavascriptFunction::E_Up:
		HTMLMenu->ExecuteJavascript("keyUp();");
		break;
	case EHTML_JavascriptFunction::E_Down:
		HTMLMenu->ExecuteJavascript("keyDown();");
		break;
	case EHTML_JavascriptFunction::E_Left:
		HTMLMenu->ExecuteJavascript("keyLeft();");
		break;
	case EHTML_JavascriptFunction::E_Right:
		HTMLMenu->ExecuteJavascript("keyRight();");
		break;
	case EHTML_JavascriptFunction::E_Next:
		HTMLMenu->ExecuteJavascript("keyNext();");
		break;
	case EHTML_JavascriptFunction::E_Previous:
		HTMLMenu->ExecuteJavascript("keyPrev();");
		break;
	case EHTML_JavascriptFunction::E_Choose:
		HTMLMenu->ExecuteJavascript("keyEventChoose();");
		break;
	}
}


void UHTMLMenu::LoadURL(FString NewURL)
{
	if (HTMLMenu.IsValid())
	{
		TArray<int32> ipArray;
		ipArray.Add(127);
		ipArray.Add(0);
		ipArray.Add(0);
		ipArray.Add(1);



		HTMLMenu->BindUObject("uecom", this, true);
		currentURL = NewURL;

		HTMLMenu->LoadURL(NewURL);

#if PLATFORM_ANDROID | PLATFORM_IOS
		appReactivtedDelegate();
#endif	

#if PLATFORM_IOS
		if (mobileActivityActor == nullptr) {
			mobileActivityActor = GetWorld()->SpawnActor(AActor::StaticClass());

			UApplicationLifecycleComponent* comp = NewObject<UApplicationLifecycleComponent>(mobileActivityActor, UApplicationLifecycleComponent::StaticClass());
			comp->RegisterComponent();
			comp->ApplicationHasReactivatedDelegate.AddDynamic(this, &UHTMLMenu::appReactivtedDelegate);
		}
#endif	


	}
}

void UHTMLMenu::LoadString(FString Contents, FString DummyURL)
{
	if (HTMLMenu.IsValid())
	{
		TArray<int32> ipArray;
		ipArray.Add(127);
		ipArray.Add(0);
		ipArray.Add(0);



		HTMLMenu->BindUObject("uecom", this, true);
		currentURL = DummyURL;

		HTMLMenu->LoadString(Contents, DummyURL);

#if PLATFORM_ANDROID | PLATFORM_IOS
		appReactivtedDelegate();
#endif	

#if PLATFORM_IOS
		if (mobileActivityActor == nullptr) {
			mobileActivityActor = GetWorld()->SpawnActor(AActor::StaticClass());

			UApplicationLifecycleComponent* comp = NewObject<UApplicationLifecycleComponent>(mobileActivityActor, UApplicationLifecycleComponent::StaticClass());
			comp->RegisterComponent();
			comp->ApplicationHasReactivatedDelegate.AddDynamic(this, &UHTMLMenu::appReactivtedDelegate);
			comp->ApplicationWillDeactivateDelegate.AddDynamic(this, &UHTMLMenu::appEnterBackgroundDelegate);
		}
#endif	


	}
}

void UHTMLMenu::ExecuteJavascript(const FString& ScriptText)
{
	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, TEXT("ExecuteJavascript 1"));
	if (HTMLMenu.IsValid())
	{
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, ScriptText);*/
		return HTMLMenu->ExecuteJavascript(ScriptText);
	}
}

FText UHTMLMenu::GetTitleText() const
{
	if (HTMLMenu.IsValid())
	{
		return HTMLMenu->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UHTMLMenu::GetUrl() const
{
	if (HTMLMenu.IsValid())
	{
		return HTMLMenu->GetUrl();
	}

	return FString();
}

void UHTMLMenu::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	HTMLMenu.Reset();

	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("bReleaseChildren"));*/

		//if (bReleaseChildren && websocketServerThread != nullptr) {
		//	websocketServerThread->stopThread();
		//	websocketServerThread = nullptr;
		//}

}

TSharedRef<SWidget> UHTMLMenu::RebuildWidget()
{

	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("RebuildWidget"));*/

	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Web Browser", "Web Browser"))
			];
	}
	else
	{
		FCreateBrowserWindowSettings browserSettings;
		browserSettings.bUseTransparency = transparentBackground;
		browserSettings.BrowserFrameRate = maxMenuFPS;
		browserSettings.bShowErrorMessage = true;
		browserSettings.InitialURL = TEXT("about:blank");

		TSharedPtr<IWebBrowserWindow> BrowserWindow;
		BrowserWindow = IWebBrowserModule::Get().GetSingleton()->CreateBrowserWindow(browserSettings);

		HTMLMenu = SNew(SWebBrowser, BrowserWindow)
			.InitialURL(TEXT("about:blank"))
			.ShowControls(false)
			.ShowErrorMessage(true)
			.SupportsTransparency(transparentBackground)
			.SupportsThumbMouseButtonNavigation(false)
			.ShowInitialThrobber(false)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged));
		return HTMLMenu.ToSharedRef();
	}
}


void UHTMLMenu::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("SynchronizeProperties"));
	//if (HTMLMenu.IsValid())
	//{

	//}
}

void UHTMLMenu::handleWebsocketData(FString& s) {
	//UE_LOG(LogTemp, Display, TEXT("URLChange1:%s"), *s);
	if (s.Contains("#") == false) {
		return;
	}

	//touch events
	if (s.Contains("#touch")) {
		TArray<FString> args;
		s.ParseIntoArray(args, TEXT("'();:"), true);
		if (args.Num() < 4) {
			return;
		}
		if (args[1].Equals("javascriptmouseposition")) {
			if (*args[2] != nullptr && *args[3] != nullptr && FCString::IsNumeric(*args[2]) && FCString::IsNumeric(*args[3]))
				javascriptMousePosition(FCString::Atof(*args[2]), FCString::Atof(*args[3]));
			return;
		}

		if (args[1].Equals("triggerue4eventmousedown") && args.Num() == 5) {
			if (*args[2] != nullptr && *args[3] != nullptr && *args[4] != nullptr && FCString::IsNumeric(*args[2]) && FCString::IsNumeric(*args[3]) && FCString::IsNumeric(*args[4]))
				triggerUE4EventMouseDown(FCString::Atof(*args[2]), FCString::Atof(*args[3]), FCString::Atoi(*args[4]));
			return;
		}

		if (args[1].Equals("triggerue4eventmouseup") && args.Num() == 5) {
			if (*args[2] != nullptr && *args[3] != nullptr && *args[4] != nullptr && FCString::IsNumeric(*args[2]) && FCString::IsNumeric(*args[3]) && FCString::IsNumeric(*args[4]))
				triggerUE4EventMouseUp(FCString::Atof(*args[2]), FCString::Atof(*args[3]), FCString::Atoi(*args[4]));
			return;
		}
		return;
	}

	//javascript event
	if (s.Contains("#js")) {
		FString left;
		FString right;
		s.Split("#js", &left, &right);
		//UE_LOG(LogTemp, Display, TEXT("URLChange2:%s"), *left);
		//UE_LOG(LogTemp, Display, TEXT("URLChange3:%s"), *right);
		s = right;//base64Decode(right);

		//UE_LOG(LogTemp, Display, TEXT("URLChange4:%s"), *s);
		if (s.Contains("UE4HTMLMenuUE4ConnectorOnUrlChange") == false) {
			return;
		}

		TArray<FString> args;
		s.ParseIntoArray(args, TEXT("'();:"), true);
		if (args.Num() < 3) {
			return;
		}
		if (args[1].Equals("triggerue4eventwithtag")) {
			switch (args.Num()) {
			case 3:
				triggerUE4EventWithTag(args[2], "", "", "", "");
				break;
			case 4:
				triggerUE4EventWithTag(args[2], args[3], "", "", "");
				break;
			case 5:
				triggerUE4EventWithTag(args[2], args[3], args[4], "", "");
				break;
			case 6:
				triggerUE4EventWithTag(args[2], args[3], args[4], args[5], "");
				break;
			case 7:
				triggerUE4EventWithTag(args[2], args[3], args[4], args[5], args[6]);
				break;
			}
			return;
		}


		if (args[1].Equals("triggerue4eventwithblank")) {
			if (args.Num() > 3) {
				triggerUE4EventWithBlank(args[2], args[3]);
			}
			else {
				triggerUE4EventWithBlank(args[2], "");
			}
			return;
		}

		if (args[1].Equals("triggerue4eventwithmap")) {
			if (args.Num() > 3) {
				triggerUE4EventWithMap(args[2], args[3]);
			}
			else {
				triggerUE4EventWithMap(args[2], "");
			}
			return;
		}

		if (args[1].Equals("triggerue4eventwithjson")) {
			if (args.Num() > 3) {
				triggerUE4EventWithJSON(args[2], args[3]);
			}
			else {
				triggerUE4EventWithJSON(args[2], "");
			}
			return;
		}
		return;
	}

	//graphics
	if (s.Contains("#ue4getgraphics")) {
		javascriptGetGraphics();
		return;
	}

	if (s.Contains("#ue4applyandsavegraphics")) {
		TArray<FString> args;
		s.ParseIntoArray(args, TEXT("'();:"), true);
		if (args.Num() < 6) {
			return;
		}

		javascriptApplyAndSaveGraphics(args[1], FCString::Atoi(*args[2]), FCString::Atoi(*args[3]), FCString::Atoi(*args[4]), FCString::Atof(*args[5]));
		return;
	}




	//console
	if (s.Contains("#console")) {
		TArray<FString> args;
		s.ParseIntoArray(args, TEXT("'();:"), true);
		if (args.Num() < 2) {
			return;
		}
		javascriptConsoleCommand(args[1]);
		return;
	}

	//end game
	if (s.Contains("#ue4quit")) {
		javascriptQuitGame();
		return;
	}

	//change level
	if (s.Contains("#ue4travel")) {
		TArray<FString> args;
		s.ParseIntoArray(args, TEXT("'();:"), true);
		if (args.Num() < 2) {
			return;
		}
		javascriptClientTravel(args[1]);
		return;
	}

}


TArray<int32> UHTMLMenu::checkIpAndPort(FString IP, int32 portMin, int32 portMax) {


	//simple ip check
	TArray<FString> ipStringArray;
	int32 lineCount = IP.ParseIntoArray(ipStringArray, TEXT("."), true);
	if (lineCount != 4) {
		UE_LOG(LogTemp, Error, TEXT("Invalid IP:%s"), *IP);
		return TArray<int32>();
	}
	TArray<int32> ipArray = { FCString::Atoi(*ipStringArray[0]), FCString::Atoi(*ipStringArray[1]), FCString::Atoi(*ipStringArray[2]) ,FCString::Atoi(*ipStringArray[3]) };
	if ((ipArray[0] < 0 || ipArray[0] > 255) || (ipArray[1] < 0 || ipArray[1] > 255) || (ipArray[2] < 0 || ipArray[2] > 255) || (ipArray[3] < 0 || ipArray[3] > 255)) {
		UE_LOG(LogTemp, Error, TEXT("Invalid IP:%s"), *IP);
		return TArray<int32>();
	}

	//simple port check
	if (portMin < 0 || portMin > 65535) {
		UE_LOG(LogTemp, Error, TEXT("Invalid port:%i"), portMin);
		return TArray<int32>();
	}
	if (portMax < 0 || portMax > 65535) {
		UE_LOG(LogTemp, Error, TEXT("Invalid port:%i"), portMax);
		return TArray<int32>();
	}
	return ipArray;
}

void UHTMLMenu::HandleOnUrlChanged(const FText& InText) {
	FString url = InText.ToString();
	if (lastUrl.IsEmpty()) {
		lastUrl = url;
		return;
	}
	if (lastUrl.Equals(url)) {
		return;
	}
	lastUrl = url;

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Purple, *url);

#if PLATFORM_ANDROID | PLATFORM_IOS
	appReactivtedDelegate();
#endif	
}

FString UHTMLMenu::base64Decode(FString text) {
	TArray<uint8> fileData;
	FBase64::Decode(*text, fileData);
	fileData.Add('\0');
	return FString(UTF8_TO_TCHAR(fileData.GetData()));
}


bool UHTMLMenu::HandleOnBeforePopup(FString URL, FString Frame)
{
	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// Retry on the GameThread.
			TWeakObjectPtr<UHTMLMenu> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
				{
					if (WeakThis.IsValid())
					{
						WeakThis->HandleOnBeforePopup(URL, Frame);
					}
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		}

		return true;
	}

	return false;
}

#if WITH_EDITOR

const FText UHTMLMenu::GetPaletteCategory()
{
	return LOCTEXT("HTML Menu", "HTML Menu");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
