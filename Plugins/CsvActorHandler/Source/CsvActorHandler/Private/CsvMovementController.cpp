// Fill out your copyright notice in the Description page of Project Settings.


#include "CsvMovementController.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CsvActorHandler/Private/CsvParser.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"


bool UCsvMovementController::InitWithTag(const FString& ActorTag, const FString& FilePath, const FString& DelimiterPosition, const FString DelimiterCoordinate)
{
	_actor = nullptr;
	if (GetOuter())
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsWithTag(GetOuter(), FName(*ActorTag), actors);
		if (actors.Num())
			_actor = actors[0];			
	}

	return InitWithActor(_actor, FilePath, DelimiterPosition, DelimiterCoordinate);
}

bool UCsvMovementController::InitWithActor(AActor* Actor, const FString& FilePath, const FString& DelimiterPosition, const FString DelimiterCoordinate)
{
	_actor = Actor;

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!platformFile.FileExists(*FilePath) || !_actor || !GetOuter())
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no camera or the file %s doesn't exist"), *FilePath)
			return false;
	}

	FTimerManager& timermanager = GetOuter()->GetWorld()->GetTimerManager();
	if (timermanager.IsTimerActive(_positionExracterHandle))
		timermanager.ClearTimer(_positionExracterHandle);
	KillParserThread();
	_positions.Empty();

	_parserThread = MakeShared<FCsvParserThread>(FilePath, _positions, DelimiterPosition, DelimiterCoordinate);
	FString name = "Parser thread";
	FRunnableThread* thread = FRunnableThread::Create(_parserThread.Get(), *name);
	_runnableThread = MakeShareable(thread);

	_cameraComponent = _actor->FindComponentByClass<UCameraComponent>();

	return true;
}

void UCsvMovementController::BeginDestroy()
{
	KillParserThread();

	Super::BeginDestroy();
}

void UCsvMovementController::KillParserThread()
{
	if (_parserThread.IsValid())
		_parserThread->Finish();
	if (_runnableThread.IsValid())
		_runnableThread->WaitForCompletion();

	_parserThread.Reset();
	_runnableThread.Reset();
}

void UCsvMovementController::Start(int32 Frequency, bool InLoop /* = true*/, float CameraFOVMultiplier /* = 0.f*/, float FOV /*= 90.f*/, float FOVMin /* = 15.f*/, float FOVMax /*= 180.f*/)
{
	_cameraFOVMultiplier = CameraFOVMultiplier;
	_FOVMin = FOVMin;
	_FOVMax = FOVMax;

	if (_cameraComponent)
		_cameraComponent->SetFieldOfView(FOV);

	if (Frequency > 0 && GetOuter())
	{
		FTimerManager& timerManager = GetOuter()->GetWorld()->GetTimerManager();
		float rate = 1.0f / Frequency;
		timerManager.SetTimer(_positionExracterHandle, this, &UCsvMovementController::SetNewActorPosition, rate, InLoop, rate);
	}
}

void UCsvMovementController::Stop()
{
	if (GetOuter())
		GetOuter()->GetWorld()->GetTimerManager().ClearTimer(_positionExracterHandle);
}

void UCsvMovementController::Pause()
{
	if (GetOuter())
		GetOuter()->GetWorld()->GetTimerManager().PauseTimer(_positionExracterHandle);
}

void UCsvMovementController::UnPause()
{
	if (GetOuter())
		GetOuter()->GetWorld()->GetTimerManager().UnPauseTimer(_positionExracterHandle);
}

void UCsvMovementController::SetNewActorPosition()
{
	float prevX = _nextPosition.X;
	if (_positions.Dequeue(_nextPosition) && _actor)
	{
		_actor->SetActorLocation(_nextPosition);

		if (_cameraComponent && !FMath::IsNearlyZero(_cameraFOVMultiplier))
			_cameraComponent->SetFieldOfView(FMath::Clamp(_cameraFOVMultiplier * (_nextPosition.X - prevX) + _cameraComponent->FieldOfView, _FOVMin, _FOVMax));
	}
}
