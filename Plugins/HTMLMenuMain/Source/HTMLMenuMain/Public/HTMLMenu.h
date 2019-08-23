// Copyright 2018-2019 David Romanski (Socke). All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Core/Public/Misc/SecureHash.h"
#include "Runtime/Core/Public/Misc/Base64.h"
#include "Runtime/Engine/Classes/Components/ApplicationLifecycleComponent.h"
#include "HTMLMenu.generated.h"

class UJavascriptBean;
class UMaterial;
class FHtmlMenuWebsocketServerThread;
class FHtmlMenuWebsocketClientThread;

UENUM(BlueprintType)
enum class EHTML_JavascriptFunction : uint8
{
	E_Up 			UMETA(DisplayName = "keyUP"),
	E_Down			UMETA(DisplayName = "keyDown"),
	E_Left 			UMETA(DisplayName = "keyLeft"),
	E_Right			UMETA(DisplayName = "keyRight"),
	E_Next			UMETA(DisplayName = "nexElement"),
	E_Previous		UMETA(DisplayName = "previousElement"),
	E_Choose		UMETA(DisplayName = "click")
};


UCLASS()
class HTMLMENUMAIN_API UHTMLMenu : public UWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FJavascriptEventMouseDown, const float, X, const float, Y, const int32, button);
	UFUNCTION()
		void JavascriptEventMouseDown(const float X, const float Y, const int32 button);
	UPROPERTY(BlueprintAssignable, Category = "HTML Menu|Events|MouseDown")
		FJavascriptEventMouseDown onJavascriptEventMouseDown;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FJavascriptEventMouseUp, const float, X, const float, Y, const int32, button);
	UFUNCTION()
		void JavascriptEventMouseUp(const float X, const float Y, const int32 button);
	UPROPERTY(BlueprintAssignable, Category = "HTML Menu|Events|MouseUp")
		FJavascriptEventMouseUp onJavascriptEventMouseUp;


	/**
	* Register Javascript event so that it can be fired in blueprints.
	*
	* @param nameOfJavascriptFunction If you call an event in Javascript with triggerUE4Event('myJavascriptEvent') then you have to put myJavascriptEvent in this field.
	* @param eventTarget This is the target for the Blueprint Event.
	*/
	UFUNCTION(BlueprintCallable, Category = "HTML Menu")
		void registerJavascriptEvent(const FString nameOfJavascriptFunction, UJavascriptBean*& eventTarget);

	UFUNCTION()
		void triggerUE4EventWithTag(FString javascriptFunctionsName, FString ID, FString className, FString value, FString arguments);
	UFUNCTION()
		void triggerUE4EventWithBlank(FString javascriptFunctionsName, FString arguments);
	UFUNCTION()
		void triggerUE4EventWithMap(FString javascriptFunctionsName, FString arguments);
	UFUNCTION()
		void triggerUE4EventWithJSON(FString javascriptFunctionsName, FString json);
	UFUNCTION()
		void triggerUE4EventMouseDown(float x, float y, int32 button);
	UFUNCTION()
		void triggerUE4EventMouseUp(float x, float y, int32 button);

	/*Javascript functions start*/

	UFUNCTION()
		void javascriptConsoleCommand(FString command);
	UFUNCTION()
		void javascriptQuitGame();
	UFUNCTION()
		void javascriptApplyAndSaveGraphics(FString resolution, int32 windowMode, int32 antiAliasing, int32 shadows, float resolutionScale);
	UFUNCTION()
		void javascriptGetGraphics();
	UFUNCTION()
		void javascriptClientTravel(FString mapOrAdress);
	UFUNCTION()
		void javascriptMousePosition(float x, float y);

	/*Javascript functions end*/


	UFUNCTION(BlueprintCallable, Category = "HTML Menu")
		void getHTMLMouseCursorPosition(float& x, float& y);

	UPROPERTY(EditAnywhere, Category = "HTML Menu settings")
		bool transparentBackground = true;

	UPROPERTY(EditAnywhere, Category = "HTML Menu settings", meta = (ClampMin = "10", ClampMax = "60"))
		int32 maxMenuFPS = 60;

	/**
	* Creates a new HTML file. All. js, css files and images will be inserted. It's all in a single HTML file. This file works even if the game is packed (. pak file). Deletes \t, \n, \r characters. Please do not use \\ comments in your css,js and html.
	*
	* @param localSourceHTML_File The HTML to convert
	* @param relativeTargetHTML_Directory The new HTML file is created in this directory
	* @param overwriteExistingFile Overwrite existing file
	*/
	UFUNCTION(BlueprintCallable, Category = "HTML Menu")
		UHTMLMenu* generateHTML_File(FString localSourceHTML_File, FString relativeTargetHTML_Directory, bool overwriteExistingFile);

	/**
	*
	*
	* @param
	*/
	UFUNCTION(BlueprintCallable, Category = "HTML Menu|Javascript")
		void triggerKeyAction(EHTML_JavascriptFunction function);

	/**
	* Loads an HTML file.
	*
	* @param htmlFile HTML File
	* @param removeLineBreaks If the file contains line breaks, they must be removed to load the page.
	*/
	UFUNCTION(BlueprintCallable, Category = "HTML Menu")
		UHTMLMenu* loadHTML_File(FString htmlFile = FString("Content/"), bool removeLineBreaks = false);

	/**
	* Load the specified URL
	*
	* @param NewURL New URL to load
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void LoadURL(FString NewURL);

	/**
	* Load a string as data to create a web page
	*
	* @param Contents String to load
	* @param DummyURL Dummy URL for the page
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void LoadString(FString Contents, FString DummyURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void ExecuteJavascript(const FString& ScriptText);

	/**
	* Get the current title of the web page
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		FText GetTitleText() const;

	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		FString GetUrl() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnUrlChanged OnUrlChanged;

	/** Called when a popup is about to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnBeforePopup OnBeforePopup;

	static TArray<int32> checkIpAndPort(FString IP, int32 portMin, int32 portMax);

	FString currentURL;
	FString base64Decode(FString text);
	void handleWebsocketData(FString& data);

	UFUNCTION()
		void appReactivtedDelegate();
	UFUNCTION()
		void appEnterBackgroundDelegate();

	int32 lastWebsocketPort = 0;

	void setWebsocketPort(int32 port);
	int32 getWebsocketPort();

public:

	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

	UMaterial* GetDefaultMaterial() const;
protected:
	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category = Appearance)
		bool bSupportsTransparency;

	void HandleOnUrlChanged(const FText& Text);

	TSharedPtr<class SWebBrowser> HTMLMenu;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
	bool HandleOnBeforePopup(FString URL, FString Frame);

private:
	FString lastUrl;
	FString widgetID;
	static TMap<FString, UJavascriptBean*> javascriptBeans;

	void createAndMinifyHTMLMenu(FString sourceHTML_File, FString targetDir);
	FString replaceCSSImages(FString content, FString cssURL);
	FString replaceCSSFonts(FString content, FString cssURL);
	FString replaceHTMLImages(FString content, FString htmlURL);
	FString removeJavascriptComments(FString content);

	float htmlMouseCursorPositionX;
	float htmlMouseCursorPositionY;

	UMaterial* DefaultMaterial;
	class FHtmlMenuWebsocketServerThread* websocketServerThread = nullptr;

	AActor* mobileActivityActor = nullptr;

};

/*********************************************** WEBSOCKET MINI SERVER ***********************************************/
/*CLIENT get Data asynchronous Thread*/
class FHtmlMenuWebsocketClientThread : public FRunnable {

public:

	FHtmlMenuWebsocketClientThread(FSocket* clientSocketP, UHTMLMenu* menuP) :
		clientSocket(clientSocketP),
		menu(menuP) {
		FString threadName = "FHtmlMenuWebsocketClientThread_" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("HTMLMenu: 20"));*/

			//UE_LOG(LogTemp, Display, TEXT("TCP Connected: %s:%i"), *session.ip, session.port);
		uint32 DataSize;
		FArrayReaderPtr Datagram = MakeShareable(new FArrayReader(true));
		while (run && clientSocket != nullptr) {
			//ESocketConnectionState::SCS_Connected does not work https://issues.unrealengine.com/issue/UE-27542
			//Compare ticks is a workaround to get a disconnect. clientSocket->Wait() stop working after disconnect. (Another bug?)
			//If it doesn't wait any longer, ticks1 and ticks2 should be the same == disconnect.
			int64 ticks1 = FDateTime::Now().GetTicks();
			clientSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(0.1));
			int64 ticks2 = FDateTime::Now().GetTicks();
			bool hasData = clientSocket->HasPendingData(DataSize);
			if (!hasData && ticks1 == ticks2)
				break;
			if (hasData) {

				Datagram->Empty();
				Datagram->SetNumUninitialized(DataSize);
				int32 BytesRead = 0;
				if (clientSocket->Recv(Datagram->GetData(), Datagram->Num(), BytesRead)) {

					if (needHandshake) {

						FString recvMessage;
						char* Data = (char*)Datagram->GetData();
						Data[BytesRead] = '\0';
						recvMessage = FString(UTF8_TO_TCHAR(Data));

						/*if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("HTMLMenu: 25: %s"), *recvMessage));*/

							//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket get Handshake Data: %s"), *recvMessage);

						TArray<FString> dataLines;
						recvMessage.ParseIntoArray(dataLines, TEXT("\r\n"), true);
						for (int i = 0; i < dataLines.Num(); i++) {
							if (dataLines[i].StartsWith("Sec-WebSocket-Key")) {
								FString clientKey = dataLines[i].Replace(TEXT("Sec-WebSocket-Key: "), TEXT(""));
								//clientKey = recvMessage.Replace(TEXT("\r"), TEXT(""));
								//clientKey = recvMessage.Replace(TEXT("\n"), TEXT(""));
								FString handshake = generateHandshakeData(clientKey);
								//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket send Handshake Data: %s"), *handshake);

								FTCHARToUTF8 Convert(*handshake);
								int32 sent = 0;
								clientSocket->Send((uint8*)((ANSICHAR*)Convert.Get()), Convert.Length(), sent);
								needHandshake = false;

								/*if (GEngine)
									GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("HTMLMenu: 22: %s"), *handshake));*/
								break;
							}
						}
						continue;
					}
					//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data 1"));
					if (Datagram->Num() >= 6) {
						//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data 2"));
						TArray<uint8> data;
						TArray<uint8> key;
						uint64 firstDataByteIndex = 0;
						data.Append(Datagram->GetData(), Datagram->Num());
						uint64	websocketFrameDataSize = 0;

						//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Dataframe Size: %i"), data.Num());
						if (data[0] == 129) {
							//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data 3"));
							uint8 tmp = data[1] - 128;
							if (tmp >= 0 && tmp <= 125) {
								websocketFrameDataSize = tmp;
								key.Add(data[2]);
								key.Add(data[3]);
								key.Add(data[4]);
								key.Add(data[5]);
								firstDataByteIndex = 6;
							}
							else {
								if (tmp == 126 && Datagram->Num() >= 8) {
									//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data 4"));
									uint16 tmp16 = data[2] << 8 | data[3] << 0;
									websocketFrameDataSize = tmp16;
									key.Add(data[4]);
									key.Add(data[5]);
									key.Add(data[6]);
									key.Add(data[7]);
									firstDataByteIndex = 8;
								}
								if (tmp == 127 && Datagram->Num() >= 14) {
									//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data 5"));
									// error C4293: '<<': shift count negative or too big, undefined behavior
									//websocketFrameDataSize = data[2] << 56 | data[3] << 48 | data[4] << 40 | data[5] << 32 | data[6] << 24 | data[7] << 16 | data[8] << 8 | data[9] << 0;
									//key.Add(data[10]);
									//key.Add(data[11]);
									//key.Add(data[12]);
									//key.Add(data[13]);
									//firstDataByteIndex = 14;
								}

							}
						}
						if (websocketFrameDataSize > 0 && key.Num() == 4 && firstDataByteIndex > 0) {
							//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data 6"));
							//decode data to string
							TArray<uint8> decoded;
							int j = 0;
							for (int i = firstDataByteIndex; i < data.Num(); i++) {
								uint8 b = key[j];
								decoded.Add(data[i] ^ b);
								j++;
								if (j > 3)
									j = 0;
							}
							char* Data = (char*)decoded.GetData();
							Data[decoded.Num()] = '\0';
							FString recvMessage = FString(UTF8_TO_TCHAR(Data));
							//UE_LOG(LogTemp, Display, TEXT("HTMLMenu Websocket Data: %s"),*recvMessage);
							/*if (GEngine)
								GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("HTMLMenu: 23: %s"), *recvMessage));*/
							if (menu != nullptr)
								menu->handleWebsocketData(recvMessage);
							decoded.Empty();
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("HTMLMenu Websocket Data cannot be processed(1)."));
						}


						data.Empty();
						key.Empty();
						firstDataByteIndex = 0;
						Datagram->Empty();
						websocketFrameDataSize = 0;
					}
					else {
						UE_LOG(LogTemp, Warning, TEXT("HTMLMenu Websocket Data cannot be processed(2)."));
					}
				}
			}
		}

		UE_LOG(LogTemp, Display, TEXT("Websocket Disconnect"));

		run = false;
		//clean up socket
		if (clientSocket != nullptr) {
			clientSocket->Close();
			ISocketSubsystem* sSS = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
			if (sSS != nullptr)
				sSS->DestroySocket(clientSocket);
			clientSocket = nullptr;
		}
		thread = nullptr;

		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("END B"));*/
		return 0;
	}

public:
	void closeClientConnection() {
		if (run) {
			run = false;
			if (clientSocket != nullptr) {
				clientSocket->Close();
			}
		}
	}

	bool isRun() {
		return run;
	}


private:
	FSocket* clientSocket = nullptr;
	FRunnableThread* thread = nullptr;

	bool	needHandshake = true;
	bool	run = true;
	UHTMLMenu* menu = nullptr;

	FString generateHandshakeData(FString clientKey) {
		FString handshakeData = "HTTP/1.1 101 \r\nUpgrade: websocket\r\nConnection: upgrade\r\nSec-WebSocket-Accept: ";
		clientKey += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		FSHAHash Hash;
		FTCHARToUTF8 Convert(*clientKey);
		FSHA1::HashBuffer((uint8*)((ANSICHAR*)Convert.Get()), Convert.Length(), Hash.Hash);
		TArray<uint8> sha1EncodedBytes;
		sha1EncodedBytes.Append(Hash.Hash, 20);
		handshakeData += FBase64::Encode(sha1EncodedBytes) + "\r\n\r\n";
		return handshakeData;


		/*	FTCHARToUTF8 Convert(*clientKey);
			FSHA1::HashBuffer((uint8*)((ANSICHAR*)Convert.Get()), Convert.Length(), outbuffer.GetData());*/
	}
};



/*SERVER start and create client sockets asynchronous Thread*/
class FHtmlMenuWebsocketServerThread : public FRunnable {

public:

	FHtmlMenuWebsocketServerThread(UHTMLMenu* menuP) :
		menu(menuP) {
		FString threadName = "FHtmlMenuWebsocketServerThread_" + FGuid::NewGuid().ToString();
		thread = FRunnableThread::Create(this, *threadName, 0, EThreadPriority::TPri_Normal);
	}

	virtual uint32 Run() override {

		ISocketSubsystem* socketSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		while (run && socketSubSystem != nullptr && websocketPortMin <= websocketPortMax) {

			listenerSocket = socketSubSystem->CreateSocket(NAME_Stream, *FString("UHTMLMenuWebsocketServer"));

			if (listenerSocket != nullptr) {
				FIPv4Endpoint BoundEndpoint = FIPv4Endpoint(FIPv4Address(127, 0, 0, 1), websocketPortMin);

				if (!listenerSocket->Bind(*BoundEndpoint.ToInternetAddr())) {
					if (listenerSocket != nullptr) {
						listenerSocket->Close();
						if (socketSubSystem != nullptr)
							socketSubSystem->DestroySocket(listenerSocket);
						listenerSocket = nullptr;
						websocketPortMin++;
						FPlatformProcess::Sleep(0.2);
						continue;
					}
				}


				if (!listenerSocket->Listen(8)) {
					if (listenerSocket != nullptr) {
						listenerSocket->Close();
						if (socketSubSystem != nullptr)
							socketSubSystem->DestroySocket(listenerSocket);
						listenerSocket = nullptr;
					}
				}
			}

			if (listenerSocket == nullptr) {
				websocketPortMin++;
				FPlatformProcess::Sleep(0.2);
				continue;
			}
			if (menu == nullptr) {
				break;
			}


			bool connected = false;
			int32 connectionAttempts = 20;
			while (run && listenerSocket != nullptr) {
				bool pending;
				listenerSocket->HasPendingConnection(pending);
				if (pending) {
					connected = true;
					UE_LOG(LogTemp, Display, TEXT("TCP Client: Pending connection"));

					/*if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("pending connection"));*/

					if (socketSubSystem != nullptr) {
						TSharedRef<FInternetAddr> remoteAddress = socketSubSystem->CreateInternetAddr();
						FSocket* clientSocket = listenerSocket->Accept(*remoteAddress, FGuid::NewGuid().ToString());
						websocketClient = new FHtmlMenuWebsocketClientThread(clientSocket, menu);
						if (menu != nullptr) {
							menu->setWebsocketPort(websocketPortMin);
						}
					}
				}
				else {
					if (!connected && menu != nullptr) {
						FPlatformProcess::Sleep(0.2);
						/*if (GEngine)
							GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::White, TEXT("openWebsocket") + FString::FromInt(websocketPortMin));*/
						menu->ExecuteJavascript("openWebsocket(" + FString::FromInt(websocketPortMin) + ");");
						//FPlatformProcess::Sleep(0.2);
						if (connectionAttempts < 0)
							run = false;
						connectionAttempts--;
					}
				}
				listenerSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(0.1));
				//we have only one client if he is death then close the server too on this port
				if (websocketClient != nullptr) {
#if PLATFORM_ANDROID
					if (websocketClient->isRun() == false) {
						run = false;
						FPlatformProcess::Sleep(5);
					}
#endif	
#if PLATFORM_IOS
					if (websocketClient->isRun() == false || menu == nullptr) {
						run = false;
						FPlatformProcess::Sleep(2);
					}
					else {
						if (menu->IsPendingKill() || menu->getWebsocketPort() != websocketPortMin) {
							run = false;
							FPlatformProcess::Sleep(2);
						}
					}
#endif	

				}
			}

		}
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("close connection"));*/

		if (listenerSocket != nullptr)
			listenerSocket->Close();
		if (socketSubSystem != nullptr && listenerSocket != nullptr)
			socketSubSystem->DestroySocket(listenerSocket);
		if (websocketClient != nullptr) {
			websocketClient->closeClientConnection();
			websocketClient = nullptr;
		}
		FPlatformProcess::Sleep(1);
		listenerSocket = nullptr;
		thread = nullptr;

		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, TEXT("END A"));*/

		return 0;
	}

public:
	void stopThread() {
		run = false;
		if (listenerSocket != nullptr) {
			listenerSocket->Close();
		}
	}

	//schreibe den aktuellen port in eine globale int variable und erhöhe min und max immer um diesen wert beim verbinden und ernidriege ihn beim abbrechen der verbinden
	//	benutze den gamethread dafur um probleme mit multithreading zu verhindern
	//	javascript pruefen 

private:
	FRunnableThread* thread = nullptr;
	FSocket* listenerSocket = nullptr;
	bool run = true;
	UHTMLMenu* menu = nullptr;

	TArray<int32> ipArray;
	int32 websocketPortMin = 49800;
	int32 websocketPortMax = 50000;
	//TArray<FHtmlMenuWebsocketClientThread*> clients;
	class FHtmlMenuWebsocketClientThread* websocketClient = nullptr;
};


