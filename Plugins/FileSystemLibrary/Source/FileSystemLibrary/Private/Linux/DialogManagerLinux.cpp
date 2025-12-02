// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "Linux/DialogManagerLinux.h"
#include "FileSystemLibraryLog.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Misc/Paths.h"
#include "Misc/Guid.h"
#include "HAL/FileManager.h"



bool DialogManagerLinux::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames)
{
	// Use IDesktopPlatform (editor and dev only)
	IDesktopPlatform *DesktopPlatform = FDesktopPlatformModule::Get();
	const void *ParentWindowHandle = GetOSWindowHandle();
	if (!DesktopPlatform || !ParentWindowHandle)
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("DialogManagerLinux::OpenFileDialog: Couldn't initialise the platform's references."))
		return false;
	}
	else
	{
		TArray<FString> ReturnPaths;
		
		EFileDialogFlags::Type Flags;
		if (bMultiSelect)
		{
			Flags = EFileDialogFlags::Type::Multiple;
		}
		else
		{
			Flags = EFileDialogFlags::Type::None;
		}

		// Opens the dialog window
		if (DesktopPlatform->OpenFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, FString(""), FileTypes, Flags, ReturnPaths))
		{
			// Checks that there is at least 1 path to return
			if (ReturnPaths.Num() > 0)
			{
				// Checks that the first path isn't empty (this could be improved)
				if (ReturnPaths[0] != "")
				{
					OutFilenames = ReturnPaths;
					return true;
				}
			}
		}
	}

	// Failure
	return false;
}

bool DialogManagerLinux::SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames)
{
	// Use IDesktopPlatform (editor and dev only)
	IDesktopPlatform *DesktopPlatform = FDesktopPlatformModule::Get();
	const void *ParentWindowHandle = GetOSWindowHandle();
	if (!DesktopPlatform || !ParentWindowHandle)
	{
		// Couldn't initialise the platform's references
		return false;
	}
	else
	{
		TArray<FString> ReturnPaths;
		
		// Opens the dialog window
		if (DesktopPlatform->SaveFileDialog(ParentWindowHandle, DialogTitle, DefaultPath, DefaultFile, FileTypes, EFileDialogFlags::Type::None, ReturnPaths))
		{
			// Check that the path isn't empty
			if (ReturnPaths.Num() > 0 && ReturnPaths[0] != FString(""))
			{
				OutFilenames = ReturnPaths;
				return true;
			}
		}
	}

	return false;
}

bool DialogManagerLinux::OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, bool bMultiSelect, TArray<FString>& OutDirectories)
{
	// Use IDesktopPlatform (editor and dev only)
	IDesktopPlatform *DesktopPlatform = FDesktopPlatformModule::Get();
	const void *ParentWindowHandle = GetOSWindowHandle();
	if (!DesktopPlatform || !ParentWindowHandle)
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("DialogManagerLinux::OpenDirectoryDialog: Couldn't initialise the platform's references."))
		return false;
	}
	else
	{
		if (bMultiSelect)
		{
			UE_LOG(FileSystemLibraryLog, Warning, TEXT("DialogManagerLinux::OpenDirectoryDialog: Current implementation does not support bMultiSelect=true."))
		}
		
		FString ReturnPath;
		
		// Opens the dialog window
		if (DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, DialogTitle, DefaultPath, ReturnPath))
		{
			// Checks that the returned path isn't empty
			if (ReturnPath != FString(""))
			{
				// Checks if the return path ends with "/"
				if (!ReturnPath.EndsWith(TEXT("/")))
				{
					ReturnPath.Append(TEXT("/"));
				}

				// Success
				OutDirectories.Add(ReturnPath);
				return true;
			}
		}
	}

	return false;
}

