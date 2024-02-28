// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/CriticalSection.h"
#include "Templates/Atomic.h"
#include "ILiveLinkSource.h"
#include "StypeLiveLinkSourceSettings.h"
#include "StypeTypes.h"
#include "UObject/ObjectMacros.h"
#include "HAL/Runnable.h"

#include "StypeLiveLinkSource.generated.h"

class ILiveLinkClient;
struct FIPv4Endpoint;
class FStypeTimingEstimator;

USTRUCT()
struct FStypeLiveLinkConnectionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	uint16 PortNumber = 54321;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FName SubjectName = "Camera";
};

class STYPELIVELINK_API FStypeLiveLinkSource : public ILiveLinkSource, public FRunnable, public TSharedFromThis<FStypeLiveLinkSource>
{
public:
	FStypeLiveLinkSource(FStypeLiveLinkConnectionSettings ConnectionSettings);

	virtual ~FStypeLiveLinkSource() override;

	// Begin ILiveLinkSource Implementation

	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
	virtual void InitializeSettings(ULiveLinkSourceSettings* Settings) override;

	void SendStaticData(bool bUsingDOF, float InWidth, float InHeight);

	virtual bool IsSourceStillValid() const override;

	virtual bool RequestSourceShutdown() override;

	virtual FText GetSourceType() const override { return SourceType; };
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override { return SourceStatus; }

	virtual TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override { return UStypeLiveLinkSourceSettings::StaticClass(); }
	virtual void OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent) override;
	// End ILiveLinkSourceImplementation

	// Begin FRunnable Interface

	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
	void Start();
	virtual void Stop() override;

	virtual void Exit() override
	{
	}

	// End FRunnable Interface

	void Send(FLiveLinkFrameDataStruct* FrameDataToSend);

private:
	ILiveLinkClient* Client;

	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;

	// Threadsafe Bool for terminating the main thread loop
	FThreadSafeBool Stopping;

	// Thread to run socket operations on
	FRunnableThread* Thread;

	// Name of the sockets thread
	FString ThreadName;

	FSocket* Socket;
	ISocketSubsystem* SocketSubsystem;

	// Receive buffer for UDP socket
	TArray<uint8> ReceiveBuffer;

	// Deferred start delegate handle.
	FDelegateHandle DeferredStartDelegateHandle;

	FStypeLiveLinkConnectionSettings ConnectionSettings;

	FLiveLinkSubjectKey SubjectKey;

	FCriticalSection DataReceivedCriticalSection;
	FFrameRate TimecodeFrameRate;
	FStypeTimingEstimator* TimingEstimator;

	float InPAWidth = -1;
	bool InbUsingDOF = false;
};
