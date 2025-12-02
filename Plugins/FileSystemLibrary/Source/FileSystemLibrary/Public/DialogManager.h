// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.


#pragma once

#include "CoreMinimal.h"


class FILESYSTEMLIBRARY_API DialogManager
{
public:
	DialogManager();
	virtual ~DialogManager();

	/**
	 * Opens a file dialog that allows the user to select files.
	 * @param DialogTitle The title to display on the file dialog window.
	 * @param DefaultPath The initial directory that the file dialog will open in.
	 * @param FileTypes A formatted string specifying the types of files that can be selected (e.g., "WAV (* .wav)|*.wav| MP3 (*.MP3)|*.mp3").
	 * @param bMultiSelect Whether the user is allowed to select multiple files (true) or only a single file (false).
	 * @param OutFilenames An array to store the file paths selected by the user.
	 * @return True if files were successfully selected, false otherwise.
	 */
	virtual bool OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames);

	/**
	 * Opens a save file dialog that allows the user to specify a file to save.
	 * @param DialogTitle The title to display on the save file dialog window.
	 * @param DefaultPath The initial directory that the save file dialog will open in.
	 * @param DefaultFile The default filename that will be suggested in the file dialog.
	 * @param FileTypes A formatted string specifying the types of files that can be saved (e.g., "Text Files (*.txt)|*.txt|All Files (*.*)|*.*").
	 * @param bMultiSelect Whether the user is allowed to select multiple files (true) or only a single file (false).
	 * @param OutFilenames An array to store the file paths specified by the user.
	 * @return True if a file was successfully specified, false otherwise.
	 */
	virtual bool SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames);

	/**
	 * Opens a directory dialog that allows the user to select directories.
	 * @param DialogTitle The title to display on the directory dialog window.
	 * @param DefaultPath The initial directory that the dialog will open in.
	 * @param MultiSelect Whether the user is allowed to select multiple directories (true) or only a single directory (false).
	 * @param OutDirectories An array to store the directory paths selected by the user.
	 * @return True if directories were successfully selected, false otherwise.
	 */
	virtual bool OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, bool bMultiSelect, TArray<FString>& OutDirectories);

protected:
	 void* GetOSWindowHandle() const;
};
