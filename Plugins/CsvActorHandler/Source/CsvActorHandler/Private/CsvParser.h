#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Containers/Queue.h"


/**
 *
 */
class CSVACTORHANDLER_API FCsvParserThread : public FRunnable
{
public:

	FCsvParserThread(const FString& FilePath, TQueue<FVector>& Positions, const FString& DelimiterPosition, const FString DelimiterCoordinate);
	virtual ~FCsvParserThread();

	virtual uint32 Run() override;

	void Finish();


private:

	const FString _filePath;
	FString _workingFileCopyPath;

	int64 _fileSize;
	int64 _positionInFile;
	TArray<uint8> _buffer;

	TQueue<FVector>& _positions;

	const FString _delimiterPosition;
	const FString _delimiterCoordinate;

	bool _keepWorking;
	float _sleepTime;

	IPlatformFile& _platformFile;

	void FillQueue();
};
