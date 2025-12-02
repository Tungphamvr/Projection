// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "FileSystemLibrary.h"

#include "FileSystemLibraryLog.h"


#if PLATFORM_WINDOWS
#include "Win/DialogManagerWin.h"
#elif PLATFORM_MAC
#include "Mac/DialogManagerMac.h"
#elif PLATFORM_ANDROID
#include "Android/DialogManagerAndroid.h"
#elif PLATFORM_LINUX
#include "Linux/DialogManagerLinux.h"
#endif


#define LOCTEXT_NAMESPACE "FFileSystemLibraryModule"


void FFileSystemLibraryModule::StartupModule()
{
	InitDialogManager();
}

void FFileSystemLibraryModule::ShutdownModule()
{
	ReleaseDialogManager();	
}


DialogManager* FFileSystemLibraryModule::GetDialogManager()
{
	FFileSystemLibraryModule& FileSystemModule = FModuleManager::GetModuleChecked<FFileSystemLibraryModule>("FileSystemLibrary");

	return FileSystemModule.DialogManager;
}

void FFileSystemLibraryModule::InitDialogManager()
{
#if PLATFORM_WINDOWS
	DialogManager = new DialogManagerWin();
#elif PLATFORM_MAC
	DialogManager = new DialogManagerMac();
#elif PLATFORM_ANDROID
	DialogManager = new DialogManagerAndroid();
#elif PLATFORM_LINUX
	DialogManager = new DialogManagerLinux();
#endif

	if (!DialogManager)
	{
		UE_LOG(FileSystemLibraryLog, Warning, TEXT("No DialogManager implemented for this platform. File dialogs will not work as intended."));
	}
}

void FFileSystemLibraryModule::ReleaseDialogManager()
{
	delete DialogManager;
	DialogManager = nullptr;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFileSystemLibraryModule, FileSystemLibrary)