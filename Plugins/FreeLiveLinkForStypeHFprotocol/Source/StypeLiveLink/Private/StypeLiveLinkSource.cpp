// Copyright Epic Games, Inc. All Rights Reserved.

#include "StypeLiveLinkSource.h"

#include "Async/Async.h"
#include "Common/UdpSocketBuilder.h"
#include "ILiveLinkClient.h"
#include "LiveLinkSourceSettings.h"
#include "Sockets.h"
#include "StypeRole.h"
#include "StypeTimingEstimator.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/Archive.h"
#include "Misc/CoreDelegates.h"
#include "StypeTypes.h"
#include "Misc/App.h"
#include <cmath>

#include "Models/SphericalLensModel.h"


#define LOCTEXT_NAMESPACE "StypeLiveLinkSource"

DEFINE_LOG_CATEGORY_STATIC(LogStypePlugin, Log, All);

namespace StypeLiveLinkSource
{
	const int32 RecvBufferSize = 1024 * 1024;
	const FTimespan SocketTimeout(FTimespan::FromMilliseconds(10));

	uint8 FloatCheckSum(float Value)
	{
		return reinterpret_cast<uint8*>(&Value)[0] + reinterpret_cast<uint8*>(&Value)[1]
			+ reinterpret_cast<uint8*>(&Value)[2] + reinterpret_cast<uint8*>(&Value)[3];
	}

	union FStypeTimecode
	{
		struct
		{
			uint8 timecode_1;
			uint8 timecode_2;
			uint8 timecode_3;
		};

		struct
		{
			uint32 frames : 7;
			uint32 seconds : 6;
			uint32 minutes : 6;
			uint32 hours : 5;
		};

		FStypeTimecode() = default;

		FStypeTimecode(uint8 tc1, uint8 tc2, uint8 tc3) : timecode_1(tc1), timecode_2(tc2), timecode_3(tc3)
		{
		}
	};

	// Structure from Stype documentation
	struct FStypePacket
	{
		static const int32 SizeOfStypePacket = 67;

		uint8 header;
		uint8 command;
		FStypeTimecode timecode;
		uint8 Package_no;
		float X;
		float Y;
		float Z;
		float Pan;
		float Tilt;
		float Roll;
		float FovX;
		float AR;
		float Focus;
		float Zoom;
		float K1;
		float K2;
		float CSX;
		float CSY;
		float PA_width;
		uint8 checksum;

		bool IsValid() const
		{
			uint8 CheckSumCounter = header + command + timecode.timecode_1 + timecode.timecode_2 + timecode.timecode_3 + Package_no
				+ FloatCheckSum(X) + FloatCheckSum(Y) + FloatCheckSum(Z) + FloatCheckSum(Pan) + FloatCheckSum(Tilt) + FloatCheckSum(Roll)
				+ FloatCheckSum(FovX) + FloatCheckSum(AR) + FloatCheckSum(Focus) + FloatCheckSum(Zoom) + FloatCheckSum(K1) + FloatCheckSum(K2)
				+ FloatCheckSum(CSX) + FloatCheckSum(CSY) + FloatCheckSum(PA_width);
			return CheckSumCounter == checksum && header == 0xF;
		}

		FArchive& Serialize(FMemoryReader& Ar)
		{
			Ar << header << command << timecode.timecode_1 << timecode.timecode_2 << timecode.timecode_3 << Package_no << X << Y << Z << Pan << Tilt << Roll
				<< FovX << AR << Focus << Zoom << K1 << K2 << CSX << CSY << PA_width << checksum;
			return Ar;
		}

		FTimecode GetTimecode(bool InbDropFrame) const
		{
			return FTimecode(timecode.hours, timecode.minutes, timecode.seconds, timecode.frames, InbDropFrame);
		}
	};
}

FStypeLiveLinkSource::FStypeLiveLinkSource(FStypeLiveLinkConnectionSettings InConnectionSettings)
	: Client(nullptr)
	  , Stopping(false)
	  , Thread(nullptr)
	  , ConnectionSettings(MoveTemp(InConnectionSettings))
	  , TimingEstimator(new FStypeTimingEstimator)
{
	SourceStatus = LOCTEXT("SourceStatus_NoData", "No data");
	SourceType = LOCTEXT("StypeSourceType", "Stype");
	SourceMachineName = FText::Format(LOCTEXT("StypeMachineName", "{0}:{1}"), FIPv4Address::Any.ToText(), FText::AsNumber(InConnectionSettings.PortNumber, &FNumberFormattingOptions::DefaultNoGrouping()));


	FIPv4Address DeviceEndpoint = FIPv4Address::Any;

	Socket = FUdpSocketBuilder(TEXT("STYPESOCKET"))
	         .AsNonBlocking()
	         .AsReusable()
	         .BoundToAddress(DeviceEndpoint)
	         .BoundToPort(ConnectionSettings.PortNumber)
	         .WithReceiveBufferSize(StypeLiveLinkSource::RecvBufferSize);

	if ((Socket != nullptr) && (Socket->GetSocketType() == SOCKTYPE_Datagram))
	{
		ReceiveBuffer.SetNumUninitialized(StypeLiveLinkSource::RecvBufferSize);
		SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		DeferredStartDelegateHandle = FCoreDelegates::OnEndFrame.AddRaw(this, &FStypeLiveLinkSource::Start);

		UE_LOG(LogStypePlugin, Log, TEXT("StypeLiveLinkSource: Opened UDP socket with IP address %s"), *DeviceEndpoint.ToString());
	}
	else
	{
		UE_LOG(LogStypePlugin, Error, TEXT("StypeLiveLinkSource: Failed to open UDP socket with IP address %s"), *DeviceEndpoint.ToString());
	}
}

FStypeLiveLinkSource::~FStypeLiveLinkSource()
{
	delete TimingEstimator;
	TimingEstimator = nullptr;

	// This could happen if the object is destroyed before FCoreDelegates::OnEndFrame calls FStypeLiveLinkSource::Start
	if (DeferredStartDelegateHandle.IsValid())
	{
		FCoreDelegates::OnEndFrame.Remove(DeferredStartDelegateHandle);
	}

	Stop();

	if (Socket != nullptr)
	{
		Socket->Close();
	}
	
	if (Thread != nullptr)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
	if(Socket != nullptr)
	{
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void FStypeLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	Client = InClient;
	SubjectKey = FLiveLinkSubjectKey(InSourceGuid, ConnectionSettings.SubjectName);
}

void FStypeLiveLinkSource::InitializeSettings(ULiveLinkSourceSettings* Settings)
{
	ILiveLinkSource::InitializeSettings(Settings);

	UStypeLiveLinkSourceSettings* StypeSettings = Cast<UStypeLiveLinkSourceSettings>(Settings);
	if (StypeSettings)
	{
		FScopeLock Lock(&DataReceivedCriticalSection);
		TimecodeFrameRate = StypeSettings->DefaultFrameRate;
	}
	else
	{
		UE_LOG(LogStypePlugin, Warning, TEXT("Stype Source coming from Preset is outdated. Consider recreating a StypeSource. Configure it and resave as preset"));
	}
}

void FStypeLiveLinkSource::SendStaticData(bool bUsingDOF, float InWidth, float InHeight)
{
	FLiveLinkStaticDataStruct StypeStaticDataStruct(FStypeLiveLinkStaticData::StaticStruct());
	FStypeLiveLinkStaticData* StypeStaticData = StypeStaticDataStruct.Cast<FStypeLiveLinkStaticData>();

	USphericalLensModel* Model = USphericalLensModel::StaticClass()->GetDefaultObject<USphericalLensModel>();
	StypeStaticData->LensModel = Model->GetModelName();

	StypeStaticData->bIsFieldOfViewSupported = true;
	StypeStaticData->bIsAspectRatioSupported = true;
	StypeStaticData->bIsProjectionModeSupported = true;
	StypeStaticData->bIsFocalLengthSupported = true;

	StypeStaticData->bIsApertureSupported = false;
	StypeStaticData->bIsFocusDistanceSupported = bUsingDOF;

	StypeStaticData->FilmBackWidth = InWidth;
	StypeStaticData->FilmBackHeight = InHeight;

	
	if (!Stopping && !(Client == nullptr))
    {
		Client->PushSubjectStaticData_AnyThread(SubjectKey, ULiveLinkStypeRole::StaticClass(), MoveTemp(StypeStaticDataStruct));	
    }
}

void FStypeLiveLinkSource::OnSettingsChanged(ULiveLinkSourceSettings* Settings, const FPropertyChangedEvent& PropertyChangedEvent)
{
	ILiveLinkSource::OnSettingsChanged(Settings, PropertyChangedEvent);

	if (UStypeLiveLinkSourceSettings* StypeSettings = Cast<UStypeLiveLinkSourceSettings>(Settings))
	{
		// Stype timecode format limits this value
		if (StypeSettings->DefaultFrameRate.AsDecimal() > 127.0)
		{
			StypeSettings->DefaultFrameRate = TimecodeFrameRate;
		}
		else
		{
			FScopeLock Lock(&DataReceivedCriticalSection);
			TimecodeFrameRate = StypeSettings->DefaultFrameRate;
		}
	}
	else
	{
		UE_LOG(LogStypePlugin, Warning, TEXT("Stype Source coming from Preset is outdated. Consider recreating a StypeSource. Configure it and resave as preset"));
	}
}

void InitializeUnrealTransformFromStype(float PosX, float PosY, float PosZ, float InPan, float InTilt, float InRoll, FTransform& UnrealTransform)
{
	// Stype: -z is Camera Forward, x is Camera Right, y is Camera Up
	// Unreal: x is Camera Forward, y is Camera Right, z is Camera Up
	float Pitch = InTilt;
	float Yaw = InPan;
	float Roll = InRoll;
	FVector CameraPosition(-PosZ, PosX, PosY);
	FRotator CameraRotation(Pitch, Yaw, Roll);

	// Stype is in M, Unreal is CM
	UnrealTransform = FTransform(CameraRotation, CameraPosition * 100, FVector(1, 1, 1));
}

uint32 FStypeLiveLinkSource::Run()
{
	while (!Stopping)
	{
		if (!(Socket && Socket->Wait(ESocketWaitConditions::WaitForRead, StypeLiveLinkSource::SocketTimeout)))
		{
			continue;
		}
		uint32 PendingDataSize = 0;
		while (Socket && Socket->HasPendingData(PendingDataSize))
		{
			int32 ReceivedDataSize = 0;
			if (!(Socket && Socket->Recv(ReceiveBuffer.GetData(), StypeLiveLinkSource::RecvBufferSize, ReceivedDataSize)))
			{
				continue;
			}

			if (ReceivedDataSize != StypeLiveLinkSource::FStypePacket::SizeOfStypePacket)
			{
				continue;
			}

			FMemoryReader ArReader(ReceiveBuffer);

			StypeLiveLinkSource::FStypePacket StypePacket;
			StypePacket.Serialize(ArReader);
			if (!StypePacket.IsValid())
			{
				continue;
			}

			SourceStatus = LOCTEXT("ActiveStatus", "Active");
			
			FLiveLinkFrameDataStruct StypeFrameDataStruct(FStypeLiveLinkFrameData::StaticStruct());
			FStypeLiveLinkFrameData* StypeFrameData = StypeFrameDataStruct.Cast<FStypeLiveLinkFrameData>();

			bool bUsingDOF = StypePacket.command && 0x02;
			if (!FMath::IsNearlyEqual(StypePacket.PA_width, InPAWidth) || (InbUsingDOF != bUsingDOF))
			{
				InbUsingDOF = bUsingDOF;
				SendStaticData(bUsingDOF, StypePacket.PA_width, StypePacket.PA_width / StypePacket.AR);
				InPAWidth = StypePacket.PA_width;
			}

			StypeFrameData->Command = StypePacket.command;

			StypeFrameData->PacketNumber = StypePacket.Package_no;

			StypeFrameData->WorldTime = FPlatformTime::Seconds();

			bool bDropFrame = false;
			FTimecode Timecode = StypePacket.GetTimecode(bDropFrame);

			StypeFrameData->Frames = Timecode.Frames;
			StypeFrameData->Seconds = Timecode.Seconds;
			StypeFrameData->Minutes = Timecode.Minutes;
			StypeFrameData->Hours = Timecode.Hours;

			check(TimingEstimator);
			TimingEstimator->Update(StypePacket.Package_no, Timecode);

			if (TimingEstimator->IsValid()) // checks that the data is valid
			{
				StypeFrameData->MetaData.SceneTime = TimingEstimator->GetQualifiedFrameTime();
			}
			else
			{
				FFrameRate CurrentFrameRate;
				{
					FScopeLock Lock(&DataReceivedCriticalSection);
					CurrentFrameRate = TimecodeFrameRate;
				}

				StypeFrameData->MetaData.SceneTime = FQualifiedFrameTime(Timecode, CurrentFrameRate);
			}

			InitializeUnrealTransformFromStype(StypePacket.X, StypePacket.Y, StypePacket.Z, StypePacket.Pan, StypePacket.Tilt, StypePacket.Roll, StypeFrameData->Transform);

			StypeFrameData->FieldOfView = StypePacket.FovX; 
			StypeFrameData->AspectRatio = StypePacket.AR;
			StypeFrameData->FocalLength = StypePacket.PA_width / (2.f * std::tanf(StypePacket.FovX * (PI / 180.f) * 0.5f));
			StypeFrameData->FocusDistance = StypePacket.Focus * 100;
			StypeFrameData->ProjectionMode = ELiveLinkCameraProjectionMode::Perspective;

			
			StypeFrameData->Focus = StypePacket.Focus;
			StypeFrameData->Zoom = StypePacket.Zoom;
			StypeFrameData->K1 = StypePacket.K1;
			StypeFrameData->K2 = StypePacket.K2;
			StypeFrameData->CenterShift = FVector2D(StypePacket.CSX, StypePacket.CSY);
			StypeFrameData->ProjectionAreaWidth = StypePacket.PA_width;

			FSphericalDistortionParameters DistortionParameters;
			DistortionParameters.K1 = StypePacket.K1;
			DistortionParameters.K2 = StypePacket.K2;
			USphericalLensModel::StaticClass()->GetDefaultObject<ULensModel>()->ToArray(DistortionParameters, StypeFrameData->DistortionParameters);

			StypeFrameData->PrincipalPoint = FVector2D(-StypePacket.CSX / StypePacket.PA_width + 0.5f, StypePacket.CSY * StypePacket.AR / StypePacket.PA_width + 0.5f);
			StypeFrameData->FxFy = FVector2D(
				StypeFrameData->FocalLength / StypePacket.PA_width, // usually it's Fx * w (pixels) / W (mm), normalized is Fx / W 
				StypeFrameData->FocalLength / StypePacket.PA_width * StypePacket.AR
			);

			

			UE_LOG(LogStypePlugin, VeryVerbose, TEXT("Stype Packet: '%d', Timecode: '%s', FrameTime: %0.10f, PlatformTime: %0.10f"), StypePacket.Package_no, *StypePacket.GetTimecode(false).ToString(), StypeFrameData->MetaData.SceneTime.Time.AsDecimal(), FApp::GetCurrentTime());
						
			Send(&StypeFrameDataStruct);
		}
	}

	return 0;
}

void FStypeLiveLinkSource::Start()
{
	check(DeferredStartDelegateHandle.IsValid());

	FCoreDelegates::OnEndFrame.Remove(DeferredStartDelegateHandle);
	DeferredStartDelegateHandle.Reset();

	SourceStatus = LOCTEXT("SourceStatus_Waiting", "Waiting for connection");

	ThreadName = "Stype Receiver ";
	ThreadName.AppendInt(FAsyncThreadIndex::GetNext());

	Thread = FRunnableThread::Create(this, *ThreadName, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

void FStypeLiveLinkSource::Stop()
{
	Stopping = true;
}

bool FStypeLiveLinkSource::IsSourceStillValid() const
{
	// Source is valid if we have a valid thread
	bool bIsSourceValid = !Stopping && (Thread != nullptr) && (Socket != nullptr);
	return bIsSourceValid;
}

bool FStypeLiveLinkSource::RequestSourceShutdown()
{
	Stop();

	return true;
}

void FStypeLiveLinkSource::Send(FLiveLinkFrameDataStruct* FrameDataToSend)
{
	if (Stopping || (Client == nullptr))
	{
		return;
	}

	Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(*FrameDataToSend));
}
#undef LOCTEXT_NAMESPACE
