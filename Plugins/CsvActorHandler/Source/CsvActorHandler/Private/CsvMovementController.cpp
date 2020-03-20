// Fill out your copyright notice in the Description page of Project Settings.


#include "CsvMovementController.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CsvActorHandler/Private/CsvParser.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "Kismet/GameplayStatics.h"


bool UCsvMovementController::Init(const FString& ActorTag, const FString& FilePath, const FString& DelimiterPosition, const FString DelimiterCoordinate)
{
	_camera = nullptr;
	if (GetOuter())
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsWithTag(GetOuter(), FName(*ActorTag), actors);
		if (actors.Num())
			_camera = actors[0];			
	}

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!platformFile.FileExists(*FilePath) || !_camera)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no camera with tag %s or the file %s doesn't exist"), *ActorTag, *FilePath)
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

void UCsvMovementController::Start(int32 Frequency, bool InLoop)
{
	if (Frequency > 0 && GetOuter())
	{
		FTimerManager& timerManager = GetOuter()->GetWorld()->GetTimerManager();
		float frequency = 1.0f / Frequency;
		timerManager.SetTimer(_positionExracterHandle, this, &UCsvMovementController::SetNewCameraPosition, frequency, InLoop, frequency);
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

void UCsvMovementController::SetNewCameraPosition()
{
	if (_positions.Dequeue(_nextPosition) && _camera)
		_camera->SetActorLocation(_nextPosition);
}
