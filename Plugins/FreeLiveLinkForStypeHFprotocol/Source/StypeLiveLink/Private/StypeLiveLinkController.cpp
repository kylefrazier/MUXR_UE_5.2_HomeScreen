// #include "StypeLiveLinkController.h"
// #include "LensDistortionModelHandlerBase.h"
// #include "StypeRole.h"
// #include "StypeTypes.h"
//
// void UStypeLiveLinkController::Tick(float DeltaTime, const FLiveLinkSubjectFrameData& SubjectData)
// {
// 	if (UCameraComponent* CameraComponent = Cast<UCameraComponent>(AttachedComponent))
// 	{
// 		CameraComponent->RefreshVisualRepresentation();
// 	}
// 	Super::Tick(DeltaTime, SubjectData);
// 	if(LensDistortionHandler)
// 	{
// 		FLensDistortionState State = LensDistortionHandler->GetCurrentDistortionState();
//
// 		for (auto Param : State.DistortionInfo.Parameters)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("%lf"), Param);
// 		}
// 		UE_LOG(LogTemp, Warning, TEXT("__"));
//
// 		const FStypeLiveLinkFrameData* FrameData = SubjectData.FrameData.Cast<FStypeLiveLinkFrameData>();
// 		State.DistortionInfo.Parameters[0] = FrameData->K1;
// 		State.DistortionInfo.Parameters[1] = FrameData->K2;
//
// 		LensDistortionHandler->SetDistortionState(State);
// 		
// 		if (UCameraComponent* CameraComponent = Cast<UCameraComponent>(AttachedComponent))
// 		{
// 			CameraComponent->RefreshVisualRepresentation();
// 		}
// 	}
//
// 	
// }
//
// bool UStypeLiveLinkController::IsRoleSupported(const TSubclassOf<ULiveLinkRole>& RoleToSupport)
// {
// 	return RoleToSupport == ULiveLinkStypeRole::StaticClass();
// }
//
// // void UStypeLiveLinkController::SetSelectedSubject(FLiveLinkSubjectRepresentation LiveLinkSubject)
// // {
// // 	Super::SetSelectedSubject(LiveLinkSubject);
// // }
//
// void UStypeLiveLinkController::ApplyDistortion(ULensFile* LensFile, UCineCameraComponent* CineCameraComponent, const FLiveLinkCameraStaticData* StaticData, const FLiveLinkCameraFrameData* FrameData)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Apply distortion"));
// }
