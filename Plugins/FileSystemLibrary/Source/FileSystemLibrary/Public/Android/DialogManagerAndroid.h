// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DialogManager.h"



class FILESYSTEMLIBRARY_API DialogManagerAndroid : public DialogManager
{
public:
	virtual bool OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames) override;
	virtual bool SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames) override;
	virtual bool OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, bool bMultiSelect, TArray<FString>& OutDirectories) override;

	/* ==== Callbacks ==== */
	void HandleDialogResults(const TArray<FString>& Paths);
	void ResetResults();

private:
	/* ==== Results ==== */
	bool bRecievedResults = false;
	TArray<FString> Results;
	
	/* ===== Utils ===== */
	static TArray<FString> ConvertFileTypesToMimeTypes(const FString& FileTypes); 
	static FString GetMimeTypeFromFilePath(const FString& FilePath);
};

