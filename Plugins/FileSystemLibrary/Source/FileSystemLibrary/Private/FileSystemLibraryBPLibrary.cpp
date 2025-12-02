// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "FileSystemLibraryBPLibrary.h"
#include "FileSystemLibraryLog.h"
#include "FileSystemLibrary.h"
#include "DialogManager.h"

#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include <string>


UFileSystemLibraryBPLibrary::UFileSystemLibraryBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


/***** File Operations *****/

bool UFileSystemLibraryBPLibrary::VerifyFile(FString PathToFile)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the file exist?
	if (PlatformFile.FileExists(*PathToFile))
	{
		// Success
		return true;
	}

	// Directory doesn't exist and wasn't created
	return false;
}

bool UFileSystemLibraryBPLibrary::CopyFile(FString PathToFile, FString DestinationFilePath)
{

	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (VerifyFile(*PathToFile))
	{
		if (PlatformFile.CopyFile(*DestinationFilePath, *PathToFile, EPlatformFileRead::AllowWrite, EPlatformFileWrite::AllowRead))
		{
			return true;
		}
	}

	// Failure
	return false;
}

bool UFileSystemLibraryBPLibrary::MoveFile(FString PathToFile, FString DestinationFilePath)
{

	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (VerifyFile(*PathToFile))
	{
		if (PlatformFile.MoveFile(*DestinationFilePath, *PathToFile))
		{
			return true;
		}
	}

	// Failure
	return false;
}

bool UFileSystemLibraryBPLibrary::RenameFile(FString PathToFile, FString NewFileName)
{
	FString Path = FPaths::GetPath(PathToFile);
	FString NewPathToFile = Path + "\\" + NewFileName;

	if (MoveFile(PathToFile, NewPathToFile))
	{
		return true;
	}

	return false;
}

bool UFileSystemLibraryBPLibrary::DeleteFile(FString PathToFile)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (VerifyFile(*PathToFile))
	{
		if (PlatformFile.DeleteFile(*PathToFile))
		{
			return true;
		}
	}

	// Failure
	return false;
}


/***** Directory Operations *****/

bool UFileSystemLibraryBPLibrary::VerifyAndCreateDirectory(const FString &PathToDirectory, bool CreateDirectory)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the directory exist?
	if (!PlatformFile.DirectoryExists(*PathToDirectory))
	{
		if (CreateDirectory)
		{
			// If not create directory
			PlatformFile.CreateDirectoryTree(*PathToDirectory);

			// Check that the directory has been created
			if (PlatformFile.DirectoryExists(*PathToDirectory))
			{
				return true;
			}
		}

		// Directory doesn't exist and wasn't created
		return false;
	}

	// Directory exist
	return true;
}

bool UFileSystemLibraryBPLibrary::VerifyDirectory(const FString &PathToDirectory)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the directory exist?
	if (!PlatformFile.DirectoryExists(*PathToDirectory))
	{
		// Directory doesn't exist
		return false;
	}

	// Directory exist
	return true;
}

bool UFileSystemLibraryBPLibrary::DeleteDirectory(FString PathToDirectory)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the directory exist?
	if (PlatformFile.DirectoryExists(*PathToDirectory))
	{
		// If it does exist, delete it
		if (PlatformFile.DeleteDirectoryRecursively(*PathToDirectory))
		{
			// Success
			return true;
		}
	}

	// Failure
	return false;
}

bool UFileSystemLibraryBPLibrary::CopyDirectory(FString PathToDirectory, FString NewPathToDirectory, bool AllowOvewrite)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the directory exist?
	if (PlatformFile.DirectoryExists(*PathToDirectory))
	{
		if (VerifyAndCreateDirectory(NewPathToDirectory, true))
		{
			// If it does exist, copy to directory tree
			if (PlatformFile.CopyDirectoryTree(*NewPathToDirectory, *PathToDirectory, AllowOvewrite))
			{
				// Success
				return true;
			}
		}
	}

	// Failure
	return false;
}

bool UFileSystemLibraryBPLibrary::MoveDirectory(FString PathToDirectory, FString NewPathToDirectory, bool AllowOvewrite)
{
	if (CopyDirectory(PathToDirectory, NewPathToDirectory, AllowOvewrite))
	{
		if (DeleteDirectory(PathToDirectory))
		{
			return true;
		}
	}
	return false;
}


/***** File & Directory Operations *****/

void UFileSystemLibraryBPLibrary::OpenDirectory(FString Path)
{
	if (VerifyDirectory(Path))
	{
		FString ValidPath = Path;

		FString Fcommand;

#if PLATFORM_WINDOWS
		ValidPath.ReplaceCharInline('/', '\\', ESearchCase::IgnoreCase);

		Fcommand = TEXT("explorer ");
#endif

#if PLATFORM_MAC
		ValidPath.InsertAt(0,'"');
		ValidPath.InsertAt(ValidPath.Len(), '"');
		Fcommand = TEXT("open ");
#endif

		Fcommand.Append(ValidPath);
		std::string command = TCHAR_TO_UTF8(*Fcommand);

		system(command.c_str());
	}
}

bool UFileSystemLibraryBPLibrary::GetFileOrDirectoryProperties(FPathProperties &Properties, FString Path)
{
	FFileStatData StatData;

	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Get directory properties
	if (VerifyDirectory(Path))
	{
		StatData = PlatformFile.GetStatData(*Path);
		Properties = FPathProperties(StatData.CreationTime, StatData.AccessTime, StatData.ModificationTime, StatData.FileSize, StatData.bIsDirectory, StatData.bIsReadOnly);
		return true;
	}

	// If not a directory, check file properties
	else if (VerifyFile(Path))
	{
		StatData = PlatformFile.GetStatData(*Path);
		Properties = FPathProperties(StatData.CreationTime, StatData.AccessTime, StatData.ModificationTime, StatData.FileSize, StatData.bIsDirectory, StatData.bIsReadOnly);
		return true;
	}

	// Failure
	return false;
}

bool UFileSystemLibraryBPLibrary::GetFileOrDirectorySize(int &FileSizeBytes, FString Path)
{
	FPathProperties Properties;

	if (GetFileOrDirectoryProperties(Properties, Path))
	{
		FileSizeBytes = (int)Properties.FileSizeBytes;
		return true;
	}

	return false;
}

bool UFileSystemLibraryBPLibrary::GetFilesInDirectory(TArray<FString> &Files, FString PathToDirectory, FString ExtensionFilter, bool OnlyReturnFilenames)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the directory exist?
	if (PlatformFile.DirectoryExists(*PathToDirectory))
	{
		TArray<FString> ReturnFiles;
		FString tempExtensionFilter = ExtensionFilter;

		// Check that the directory has been created
		PlatformFile.FindFiles(ReturnFiles, *PathToDirectory, *tempExtensionFilter);

		// Check if found any files
		if (ReturnFiles.Num() > 0)
		{
			// Check if we want to exclude the extension from the return array.
			if (OnlyReturnFilenames)
			{
				TArray<FString> ReturnFilesNoExtension;

				for (int i = 0; i < ReturnFiles.Num(); i++)
				{
					ReturnFilesNoExtension.Add(FPaths::GetBaseFilename(ReturnFiles[i]));
				}

				Files = ReturnFilesNoExtension;
				return true;
			}

			//If not return the array
			else 
			{
				Files = ReturnFiles;
				return true;
			}
		}
	}

	// No files were found
	return false;
}

bool UFileSystemLibraryBPLibrary::GetFilesRecursivelyInDirectory(TArray<FString> &Files, FString PathToDirectory, FString ExtensionFilter, bool OnlyReturnFilenames)
{
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Does the directory exist?
	if (PlatformFile.DirectoryExists(*PathToDirectory))
	{
		TArray<FString> ReturnFiles;
		FString tempExtensionFilter = ExtensionFilter;

		// Check that the directory has been created
		PlatformFile.FindFilesRecursively(ReturnFiles, *PathToDirectory, *tempExtensionFilter);

		// Check if found any files
		if (ReturnFiles.Num() > 0)
		{
			// Check if we want to exclude the extension from the return array.
			if (OnlyReturnFilenames)
			{
				TArray<FString> ReturnFilesNoExtension;

				for (int i = 0; i < ReturnFiles.Num(); i++)
				{
					ReturnFilesNoExtension.Add(FPaths::GetBaseFilename(*ReturnFiles[i]));
				}

				Files = ReturnFilesNoExtension;
				return true;
			}

			//If not return the array
			else
			{
				Files = ReturnFiles;
				return true;
			}
		}
	}

	// No files were found
	return false;
}

bool UFileSystemLibraryBPLibrary::GetFoldersInDirectory(TArray<FString> &Folders, FString Path)
{
		TArray<FString> ReturnFolders;

		// Check that the directory has been created
		IFileManager::Get().FindFiles(ReturnFolders, *Path, false, true); //This is platform specific and seems to throw an error on Windows.

		// Check if found any files
		if (ReturnFolders.Num() > 0)
		{
			Folders = ReturnFolders;
			return true;
		}

	// No files were found
	return false;
}


/***** File IO *****/

bool UFileSystemLibraryBPLibrary::LoadTextFileToStringArray(TArray<FString> &FileContent, FString PathToFile)
{
	// Does the file exist?
	if (VerifyFile(*PathToFile))
	{
		TArray<FString> ReturnFileContent;

		FFileHelper::LoadFileToStringArray(ReturnFileContent, *PathToFile);

		if (ReturnFileContent.Num() > 0)
		{
			// Success
			FileContent = ReturnFileContent;
			return true;
		}
	}

	// No files were found
	return false;
}

bool UFileSystemLibraryBPLibrary::InsertStringArrayToFile(FString PathToFile, TArray<FString> FileContent, int InsertAtIndex)
{
	TArray<FString> ReturnFileContent;

	if (LoadTextFileToStringArray(ReturnFileContent, PathToFile))
	{
		ReturnFileContent.Insert(FileContent, InsertAtIndex);

		if (SaveStringArrayToFile(PathToFile, ReturnFileContent))
		{
			return true;
		}
	}

	return false;
}

bool UFileSystemLibraryBPLibrary::LoadTextFileToString(FString &FileContent, FString PathToFile)
{
	// Does the file exist?
	if (VerifyFile(*PathToFile))
	{
		TArray<FString> ReturnFileContent;

		FFileHelper::LoadFileToStringArray(ReturnFileContent, *PathToFile);

		if (ReturnFileContent.Num() > 0)
		{
			FString ReturnString;

			for (int i = 0; i < ReturnFileContent.Num(); i++)
			{
				ReturnString = ReturnFileContent[i] + '\n';
			}

			// Success
			FileContent = ReturnString;
			return true;
		}
	}

	// No files were found
	return false;
}

bool UFileSystemLibraryBPLibrary::SaveStringArrayToFile(FString PathToFile, TArray<FString> FileContent)
{
	IFileManager &FileManager = IFileManager::Get();

	TArray<FString> ReturnFileContent;
	uint32 Flags = 0;

	FFileHelper::SaveStringArrayToFile(FileContent, *PathToFile, FFileHelper::EEncodingOptions::AutoDetect, &FileManager, Flags);

	if (ReturnFileContent.Num() > 0)
	{
		// Success
		FileContent = ReturnFileContent;
		return true;
	}

	// No files were found
	return false;
}

bool UFileSystemLibraryBPLibrary::AppendStringArrayToFile(FString PathToFile, TArray<FString> FileContent, bool AppendFileToStringArray)
{
	TArray<FString> ReturnFileContent;

	if (LoadTextFileToStringArray(ReturnFileContent, PathToFile))
	{
		if (AppendFileToStringArray)
		{
			TArray<FString> InFileContent = FileContent;
			InFileContent.Append(ReturnFileContent);
			ReturnFileContent = InFileContent;
		}
		else
		{
			ReturnFileContent.Append(FileContent);
		}

		if (SaveStringArrayToFile(PathToFile, ReturnFileContent))
		{
			return true;
		}
	}

	return false;
}


/***** Path Utilities *****/

FString UFileSystemLibraryBPLibrary::GetFileExtension(FString Path)
{
	return FPaths::GetExtension(Path, false);
}

FString UFileSystemLibraryBPLibrary::GetFilePath(FString Path)
{
	return FPaths::GetPath(Path);
}

FString UFileSystemLibraryBPLibrary::GetFileName(FString Path, bool IncludeExtension)
{
	if (!IncludeExtension)
	{
		return FPaths::GetBaseFilename(Path, true);
	}

	else
	{
		return FPaths::GetCleanFilename(Path);
	}
}


/***** File Dialogs *****/

bool UFileSystemLibraryBPLibrary::OpenFolderMultiSelectDialog(TArray<FString> &Directories, FString DialogTitle, FString DefaultPath)
{
	if (DialogManager *DialogMan = FFileSystemLibraryModule::GetDialogManager())
	{
		TArray<FString> ReturnValue;
		if (DialogMan->OpenDirectoryDialog(DialogTitle, DefaultPath, true, ReturnValue))
		{
			for (FString& Directory : ReturnValue)
			{
				if (Directory != TEXT(""))
				{
					if (!Directory.EndsWith("/", ESearchCase::IgnoreCase))
					{
						Directory.Append("/");
					}

					Directories.Add(Directory);
				}
			}

			return Directories.Num() > 0;
		}
	}
	else
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("FileSystemLibrary does not currently support this platform."));
	}

	return false;
}

bool UFileSystemLibraryBPLibrary::OpenFolderSelectDialog(FString &FolderPath, FString DialogTitle, FString DefaultPath)
{
	TArray<FString> Directories;

	if (OpenFolderMultiSelectDialog(Directories, DialogTitle, DefaultPath))
	{

		FolderPath = Directories[0];
		return true;
	}
	return false;
}

bool UFileSystemLibraryBPLibrary::OpenFileMultiSelectDialog(TArray<FString> &FilePaths, FString DialogTitle, FString DefaultPath, bool AllowMultiSelect, FString FileTypes)
{
	if (DialogManager *DialogMan =  FFileSystemLibraryModule::GetDialogManager())
	{
		if (DialogMan->OpenFileDialog(DialogTitle, DefaultPath, FileTypes, AllowMultiSelect, FilePaths))
		{
			return FilePaths.Num() > 0;
		}
	}
	else
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("FileSystemLibrary does not currently support this platform."));
	}
	
	return false;
}

bool UFileSystemLibraryBPLibrary::OpenFileSelectDialog(FString &FilePath, FString DialogTitle, FString DefaultPath, FString FileTypes)
{
	TArray<FString> FilePaths;

	if (OpenFileMultiSelectDialog(FilePaths, DialogTitle, DefaultPath, false, FileTypes))
	{
		FilePath = FilePaths[0];
		return true;
	}
	return false;
}

bool UFileSystemLibraryBPLibrary::OpenSaveFileDialog(FString &SaveToPath, FString DialogTitle, FString DefaultPath, FString DefaultFileName, FString FileTypes)
{
	if (DialogManager *DialogMan = FFileSystemLibraryModule::GetDialogManager())
	{
		TArray<FString> ReturnValue;
		if (DialogMan->SaveFileDialog(DialogTitle, DefaultPath, DefaultFileName, FileTypes, false, ReturnValue))
		{
			if (ReturnValue.Num() > 0 && ReturnValue[0] != TEXT(""))
			{
				SaveToPath = ReturnValue[0];
				return true;
			}
		}
	}
	else
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("FileSystemLibrary does not currently support this platform."));
	}
	
	return false;
}


/***** Process operations *****/

bool UFileSystemLibraryBPLibrary::CreateProcess(FString PathToExecutable, FString Arguments, bool LaunchDetached, bool LaunchedHidden, bool LaunchReallyHidden, int PriorityModifier, bool UseWorkingDirectory, FString WorkingDirectory, int32& ProcessID)
{
	const TCHAR* tFilename = *PathToExecutable;
	const TCHAR* tArguments = *Arguments;
	const TCHAR* tWorkingDirectory = (UseWorkingDirectory) ? *WorkingDirectory : nullptr;
	int32 PrioMod = (int32)PriorityModifier;
	uint32 tProcessID = 0;
	FProcHandle ProcessHandle;

	#if PLATFORM_WINDOWS
	ProcessHandle = FWindowsPlatformProcess::CreateProc(tFilename, tArguments, LaunchDetached, LaunchedHidden, LaunchReallyHidden, &tProcessID, PrioMod, tWorkingDirectory, nullptr, nullptr);
	#elif PLATFORM_MAC
	ProcessHandle = FMacPlatformProcess::CreateProc(tFilename, tArguments, LaunchDetached, LaunchedHidden, LaunchReallyHidden, &tProcessID, PrioMod, tWorkingDirectory, nullptr, nullptr);
	#endif
	
	ProcessID = -1;
	if(tProcessID)
	{
		ProcessID = tProcessID;
	}
	
	return true;
}

bool UFileSystemLibraryBPLibrary::IsProcessRunning(int32 ProcessID)
{
	return FPlatformProcess::IsApplicationRunning(ProcessID);
}	

FString UFileSystemLibraryBPLibrary::GetProcessName(int32 ProcessID)
{
	return FPlatformProcess::GetApplicationName(ProcessID);
}

UCreateProcessWithCallback* UCreateProcessWithCallback::CreateProcessWithCallback(UObject* WorldContextObj, FString PathToExecutable, FString Arguments,bool LaunchDetached, bool LaunchedHidden, bool LaunchReallyHidden, int PriorityModifier, bool UseWorkingDirectory, FString WorkingDirectory)
{
	int32 tProcessID;
	UFileSystemLibraryBPLibrary::CreateProcess(PathToExecutable, Arguments,LaunchDetached,LaunchedHidden,LaunchReallyHidden, PriorityModifier,UseWorkingDirectory,WorkingDirectory, tProcessID); 
	
	auto* AsyncAction = NewObject<UCreateProcessWithCallback>();
	AsyncAction->ProcessID = tProcessID;
	AsyncAction->WorldContextObj = WorldContextObj;
	return AsyncAction;

}

void UCreateProcessWithCallback::Activate()
{
	Super::Activate();

	if(WorldContextObj && ProcessID != 0)
	{
		WorldContextObj->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCreateProcessWithCallback::bIsProcessRunning);
	}
}

void UCreateProcessWithCallback::bIsProcessRunning()
{
	bool const bIsRunning = FPlatformProcess::IsApplicationRunning(ProcessID);
	
	if(bIsRunning)
	{
		if(WorldContextObj)
		{
			WorldContextObj->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCreateProcessWithCallback::bIsProcessRunning);
		}
	}
	else
	{
		{
			Completed.Broadcast();
		}
	}
	
}
