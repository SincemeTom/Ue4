// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Core/Public/Serialization/BufferArchive.h"
#include "Runtime/CoreUObject/Public/Serialization/ArchiveUObject.h"
//#include "SaveGameArchive.generated.h"

struct FSaveGameWriteArchive : public FBufferArchive
{

	FSaveGameWriteArchive(bool bIsPersistent = false, const FName InArchiveName = NAME_None)
		:FBufferArchive(bIsPersistent, InArchiveName)
	{
		ArIsSaveGame = true;
	}

	virtual FArchive& operator<<(class UObject*& Obj) override;
}; 

class FSaveGameReadArchive final : public FMemoryArchive
{
public:
	/**
	* Returns the name of the Archive.  Useful for getting the name of the package a struct or object
	* is in when a loading error occurs.
	*
	* This is overridden for the specific Archive Types
	**/
	virtual FString GetArchiveName() const { return TEXT("FMemoryReader"); }

	int64 TotalSize()
	{
		return FMath::Min((int64) Bytes.Num(), LimitSize);
	}

	void Serialize(void* Data, int64 Num)
	{
		if (Num && !ArIsError)
		{
			// Only serialize if we have the requested amount of data
			if (Offset + Num <= TotalSize())
			{
				FMemory::Memcpy(Data, &Bytes[Offset], Num);
				Offset += Num;
			}
			else
			{
				ArIsError = true;
			}
		}
	}

	FSaveGameReadArchive(const TArray<uint8>& InBytes, bool bIsPersistent = false)
		: FMemoryArchive()
		, Bytes(InBytes)
		, LimitSize(INT64_MAX)
	{
		ArIsLoading = true;
		ArIsPersistent = bIsPersistent;
	}

	/** With this method it's possible to attach data behind some serialized data. */
	void SetLimitSize(int64 NewLimitSize)
	{
		LimitSize = NewLimitSize;
	}

	virtual FArchive& operator<<(class UObject*& Obj) override;

protected:

	const TArray<uint8>& Bytes;
	int64 LimitSize;
};
