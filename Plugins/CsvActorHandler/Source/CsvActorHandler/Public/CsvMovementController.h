// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Queue.h"
#include "Engine/EngineTypes.h"
#include "CsvMovementController.generated.h"

class FCsvParserThread;
class UCameraComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class CSVACTORHANDLER_API UCsvMovementController : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	bool InitWithTag(const FString& ActorTag, const FString& FilePath, const FString& DelimiterPosition, const FString DelimiterCoordinate);
	UFUNCTION(BlueprintCallable)
	bool InitWithActor(AActor* Actor, const FString& FilePath, const FString& DelimiterPosition, const FString DelimiterCoordinate);

	UFUNCTION(BlueprintCallable)
	void Start(int32 Frequency, bool InLoop = true, float CameraFOVmultiplier = 0.f, float FOV = 90.f, float FOVmin = 15.f, float FOVmax = 170.f);
	UFUNCTION(BlueprintCallable)
	void Stop();
	UFUNCTION(BlueprintCallable)
	void Pause();
	UFUNCTION(BlueprintCallable)
	void UnPause();

	virtual void BeginDestroy() override;

private:

	UPROPERTY()
	AActor* _actor;
	UPROPERTY()
	UCameraComponent* _cameraComponent;

	TSharedPtr<FRunnableThread> _runnableThread; 
	TSharedPtr<FCsvParserThread> _parserThread;

	float _cameraFOVMultiplier;
	float _FOVMin;
	float _FOVMax;

	FVector _nextPosition;
	TQueue<FVector> _positions;

	FTimerHandle _positionExracterHandle;

	UFUNCTION()
	void SetNewActorPosition();

	void KillParserThread();
};
