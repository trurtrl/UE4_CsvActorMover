#include "CsvParser.h"
#include "Misc/FileHelper.h"
#if PLATFORM_WINDOWS
#include "Core/Public/Windows/WindowsPlatformProcess.h"
#elif PLATFORM_ANDRPOID
#include "Core/Public/Android/AndroidPlatformProcess.h"
#elif PLATFORM_LINUX
#include "Core/Public/Linux/LinuxPlatformProcess.h"
#elif_PLATFORM_UNIX
#include "Core/Public/Unix/UnixPlatformProcess.h"
#elif PLATFORM_MAC
#include "Core/Public/Mac/MacPlatformProcess.h"
#elif PLATFORM_HOLOLENS
#include "Core/Public/HoloLens/HoloLensPlatformProcess.h"
#elif PLATFORM_LUMIN
#include "Core/Public/Lumin/LuminPlatformProcess.h"
#endif


FCsvParserThread::FCsvParserThread(IFileHandle* FileHandle, TQueue<FVector>& Positions, const FString& DelimiterPosition, const FString DelimiterCoordinate)
	: _fileHandle(FileHandle)
	, _positionInFile(0)
	, _positions(Positions)
	, _delimiterPosition(DelimiterPosition)
	, _delimiterCoordinate(DelimiterCoordinate)
	, _keepWorking(true)
	, _sleepTime(0.01f)
{

}

uint32 FCsvParserThread::Run()
{
	while (_keepWorking)
	{
		if (_fileHandle->Size() > _positionInFile)
			FillQueue();

		FPlatformProcess::Sleep(_sleepTime);
	}

	return 0;
}

void FCsvParserThread::Finish()
{
	_keepWorking = false;
}

void FCsvParserThread::FillQueue()
{
	int64 bytesToRead = _fileHandle->Size() - _positionInFile;
	if (_buffer.Num() < bytesToRead)
		_buffer.SetNum(bytesToRead);

	if (_fileHandle->Read(_buffer.GetData(), bytesToRead))
	{
		FString result;
		FFileHelper::BufferToString(result, _buffer.GetData(), bytesToRead);
		TArray<FString> positionsArray;
		result.ParseIntoArray(positionsArray, *_delimiterPosition);
		for (FString& position : positionsArray)
		{
			TArray<FString> coordinates;
			int32 coordNum = position.ParseIntoArray(coordinates, *_delimiterCoordinate);
			if (coordNum == 3)	// if coordinates for x, y, z exist
			{
				FVector positionVector(FCString::Atoi(*coordinates[0]), FCString::Atoi(*coordinates[1]), FCString::Atoi(*coordinates[2]));
				_positions.Enqueue(positionVector);
			}
		}

		_positionInFile += bytesToRead;
	}
}