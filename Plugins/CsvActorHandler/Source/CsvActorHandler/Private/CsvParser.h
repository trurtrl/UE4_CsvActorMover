#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Containers/Queue.h"

class IFileHandle;

/**
 *
 */
class CSVACTORHANDLER_API FCsvParserThread : public FRunnable
{
public:

	FCsvParserThread(IFileHandle* FileHandle, TQueue<FVector>& Positions, const FString& DelimiterPosition, const FString DelimiterCoordinate);

	virtual uint32 Run() override;

	void Finish();


private:

	IFileHandle* _fileHandle;

	int64 _positionInFile;
	TArray<uint8> _buffer;

	TQueue<FVector>& _positions;

	const FString _delimiterPosition;
	const FString _delimiterCoordinate;

	bool _keepWorking;
	float _sleepTime;

	void FillQueue();
};
