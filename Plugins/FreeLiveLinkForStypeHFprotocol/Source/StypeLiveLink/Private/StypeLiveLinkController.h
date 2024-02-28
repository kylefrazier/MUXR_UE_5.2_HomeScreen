// #pragma once
//
// #include "LiveLinkCameraController.h"
// #include "LiveLinkControllerBase.h"
//
// #include "CineCameraComponent.h"
// #include "Controllers/LiveLinkTransformController.h"
// #include "Engine/EngineTypes.h"
// #include "LensDistortionModelHandlerBase.h"
// #include "LensFile.h"
//
// #include "StypeLivelinkController.generated.h"
//
//
// UCLASS()
// class UStypeLiveLinkController : public ULiveLinkCameraController
// {
// 	GENERATED_BODY()
//
// public:
// 	virtual void Tick(float DeltaTime, const FLiveLinkSubjectFrameData& SubjectData) override;
// 	virtual bool IsRoleSupported(const TSubclassOf<ULiveLinkRole>& RoleToSupport) override;
// 	// virtual void SetSelectedSubject(FLiveLinkSubjectRepresentation LiveLinkSubject) override;
// 	
// protected:
// 	/** Update distortion state */
// 	void ApplyDistortion(ULensFile* LensFile, UCineCameraComponent* CineCameraComponent, const FLiveLinkCameraStaticData* StaticData, const FLiveLinkCameraFrameData* FrameData);
// };
