// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Queue.h"
#include "Engine/EngineTypes.h"
#include "CsvMovementController.generated.h"

class FCsvParserThread;

/**
 * 
 */
UCLASS(Blueprintable)
class CSVACTORHANDLER_API UCsvMovementController : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	bool Init(const FString& ActorTag, const FString& FilePath, const FString& DelimiterPosition, const FString DelimiterCoordinate);

	UFUNCTION(BlueprintCallable)
	void Start(int32 Frequency, bool InLoop = true);
	UFUNCTION(BlueprintCallable)
	void Stop();
	UFUNCTION(BlueprintCallable)
	void Pause();
	UFUNCTION(BlueprintCallable)
	void UnPause();

	virtual void BeginDestroy() override;

private:

	UPROPERTY()
	AActor* _camera;

	TSharedPtr<FRunnableThread> _runnableThread; 
	TSharedPtr<FCsvParserThread> _parserThread;

	FVector _nextPosition;
	TQueue<FVector> _positions;

	FTimerHandle _positionExracterHandle;

	UFUNCTION()
	void SetNewCameraPosition();

	void KillParserThread();
};
