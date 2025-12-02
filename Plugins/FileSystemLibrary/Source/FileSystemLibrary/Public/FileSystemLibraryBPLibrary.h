// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CoreMinimal.h"
#include "FileSystemLibraryBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct FILESYSTEMLIBRARY_API FPathProperties
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PathProperties")
	FDateTime CreationDate;

	UPROPERTY(BlueprintReadOnly, Category = "PathProperties")
	FDateTime AccessDate;

	UPROPERTY(BlueprintReadOnly, Category = "PathProperties")
	FDateTime ModificationDate;

	UPROPERTY(BlueprintReadOnly, Category = "PathProperties")
	int FileSizeBytes;

	UPROPERTY(BlueprintReadOnly, Category = "PathProperties")
	bool isDirectory;

	UPROPERTY(BlueprintReadOnly, Category = "PathProperties")
	bool isReadOnly;

	FPathProperties()
	{
		CreationDate = FDateTime::MinValue();
		AccessDate = FDateTime::MinValue();
		ModificationDate = FDateTime::MinValue();
		FileSizeBytes = 0;
		isDirectory = false;
		isReadOnly = false;
	}

	FPathProperties(FDateTime inCreationDate, FDateTime inAccessDate, FDateTime inModificationDate, int64 inFileSizeBytes, bool inIsDirectory, bool inIsReadOnly)
	{
		CreationDate = inCreationDate;
		AccessDate = inAccessDate;
		ModificationDate = inModificationDate;
		FileSizeBytes = int(inFileSizeBytes);
		isDirectory = inIsDirectory;
		isReadOnly = inIsReadOnly;
	}
};

/***** FileSystemLibrary Blueprint function library *****/
UCLASS()
class FILESYSTEMLIBRARY_API UFileSystemLibraryBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/***** File Operations *****/

	/* This function will check to see if the specified file (folder) exists. You need to include the file extension. 
	@param PathToFile The path to the file to verify (including extension)
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "VerifyFile", Keywords = "FileSystemLibrary"), Category = "System File Operations")
	static bool VerifyFile(FString PathToFile = "");

	/* This function will copy a file from a path to another. You need to include the full path with extension for both input parameters. 
	@param	PathToFile				Path to the file to copy (including extension).
	@param	DestinationFilePath		Path to copy the file to (including filename and extension).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CopyFile", Keywords = "FileSystemLibrary"), Category = "System File Operations")
	static bool CopyFile(FString PathToFile, FString DestinationFilePath = "");

	/* This function will copy a file from a path to another. You need to include the full path with extension for both input parameters. 
	@param	PathToFile				Path to the file to move (including extension).
	@param	DestinationFilePath		Path to move the file to (including filename and extension).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveFile", Keywords = "FileSystemLibrary"), Category = "System File Operations")
	static bool MoveFile(FString PathToFile, FString DestinationFilePath = "");

	/* This function will rename the specified file. You need to include filename with extension for both input parameters. 
	@param	PathToFile		Path to the file to rename (including extension).
	@param	NewFileName		New name for the file (including extension).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RenameFile", Keywords = "FileSystemLibrary"), Category = "System File Operations")
	static bool RenameFile(FString PathToFile, FString NewFileName = "");

	/* This function will rename the specified file. You need to include filename with extension for both input parameters. 
	@param PathToFile	Path to the file to delete (including extension).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "DeleteFile", Keywords = "FileSystemLibrary"), Category = "System File Operations")
	static bool DeleteFile(FString PathToFile);


	/***** Directory Operations *****/

	/* This function will check to see if the specified directory (folder) exist. If it doesn't and CreateDirectory=true, it will create the directory for you. 
	@param PathToDirectory	Path to the directory to verify.
	@param CreateDirectory	If true, the directory will be created if it doesn't exist.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "VerifyAndCreateDirectory", Keywords = "FileSystemLibrary"), Category = "System Directory Operations")
	static bool VerifyAndCreateDirectory(const FString &PathToDirectory = "", bool CreateDirectory = true);

	/* This function will check to see if the specified directory (folder) exist.
@param PathToDirectory	Path to the directory to verify.
*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "VerifyDirectory", Keywords = "FileSystemLibrary"), Category = "System Directory Operations")
	static bool VerifyDirectory(const FString &PathToDirectory = "");

	/* This function will the specified directory and all file/folders inside it. 
	@param PathToDirectory The path to the directory to delete.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "DeleteDirectory", Keywords = "FileSystemLibrary"), Category = "System Directory Operations")
	static bool DeleteDirectory(FString PathToDirectory = "");

	/* This function will copy all files and folders from PathToDirectory to NewPathToDirectory. 
	@param	PathToDirectory		Path to the directory to copy.
	@param	NewPathToDirectory	Path to the directory to copy the files to.
	@param	AllowOvewrite		If true, files that already exist in the destination path will be overwritten.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CopyDirectory", Keywords = "FileSystemLibrary"), Category = "System Directory Operations")
	static bool CopyDirectory(FString PathToDirectory = "", FString NewPathToDirectory = "", bool AllowOvewrite = true);

	/* This function will move all files and folders from PathToDirectory to NewPathToDirectory. 
	@param	PathToDirectory		Path to the directory to move.
	@param	NewPathToDirectory	Path to the directory to move the files to.
	@param	AllowOvewrite		If true, files that already exist in the destination path will be overwritten.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MoveDirectory", Keywords = "FileSystemLibrary"), Category = "System Directory Operations")
	static bool MoveDirectory(FString PathToDirectory = "", FString NewPathToDirectory = "", bool AllowOvewrite = true);

	/***** File & Directory Operations *****/

	
	
	/** Opens Windows' explorer or Mac OS' finder at the specified path.
	* @param Path		Path to the directory.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Open Directory", Keywords = "explorer finder folder directory"), Category = "File System Library")
		static void OpenDirectory(FString Path);
	
	/* This function will return the file's or folder's properties. 
	@param	Path		Path to the file (including extension).
	@return	Properties	The file's property.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetFileOrDirectoryProperties", Keywords = "FileSystemLibrary"), Category = "File System Library")
	static bool GetFileOrDirectoryProperties(FPathProperties &Properties, FString Path = "");

	/* This function will return the file's or folder's properties. 
	@param	Path			Path to the file (including extension).
	@return	FileSizeBytes	The file's size in bytes (multiply by 1 000 000 to get the result in Mb).
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetFileOrDirectorySize", Keywords = "FileSystemLibrary"), Category = "File System Library")
	static bool GetFileOrDirectorySize(int &FileSizeBytes, FString Path = "");

	/* This function will return the name of all files present in the specified directory. 
	@param	PathToDirectory			Path to the directory.
	@param	ExtensionFilter			If set, will only return files of the input extension. (".XXX" or "XXX").
	@param	OnlyReturnFilenames		If true, will only return the filenames (without the extension).
	@return	Files					The files found in the specific directory.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetFilesInDirectory", Keywords = "FileSystemLibrary"), Category = "File System Library")
	static bool GetFilesInDirectory(TArray<FString> &Files, FString PathToDirectory, FString ExtensionFilter, bool OnlyReturnFilenames);

	/* This function will return the name of all files present in the specified directory and all sub-directories.
	@param	PathToDirectory			Path to the directory.
	@param	ExtensionFilter			If set, will only return files of the input extension. (".XXX" or "XXX").
	@param	OnlyReturnFilenames		If true, will only return the filenames (without the extension).
	@return	Files					The files found in the specific directory.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetFilesRecursivelyInDirectory", Keywords = "FileSystemLibrary"), Category = "File System Library")
	static bool GetFilesRecursivelyInDirectory(TArray<FString> &Files, FString PathToDirectory, FString ExtensionFilter, bool OnlyReturnFilenames);
	
	/* This function will return the directories present at the specified path.
	@param	Path		Path to the directory to search in.
	@return	Folders		If true, will only return the filenames (without the extension).
	*/
	//UFUNCTION(BlueprintPure, meta = (DisplayName = "GetFoldersInDirectory", Keywords = "FileSystemLibrary"), Category = "File System Library folder")
	static bool GetFoldersInDirectory(TArray<FString> &Folders, FString Path);


	/***** File IO *****/

	/* This function will load the content of the specified file to a string array. For text file, each array element represents a line from the document.
	@param	PathToFile	Path to the file to load the string array from.
	@return	FileContent	The file's content.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "LoadTextFileToStringArray", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static bool LoadTextFileToStringArray(TArray<FString> &FileContent, FString PathToFile);

	/* This function will load the content of the specified file to a string. For text file, each array element represents a line from the document.
	@param	PathToFile		Path to file to edit.
	@param	FileContent		Content to insert (where one element of the array represent a line of the document).
	@param	InsertAtIndex	Line number to insert to content at.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "InsertStringArrayToFile", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static bool InsertStringArrayToFile(FString PathToFile, TArray<FString> FileContent, int InsertAtIndex);

	/* This function will load the content of the specified file to a string array. For text file, each array element represents a line from the document.
	@param	PathToFile		Path to the file to load the string from.
	@return	FileContent		The file's content.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "LoadTextFileToString", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static bool LoadTextFileToString(FString &FileContent, FString PathToFile);

	/* This function save the input content to a file.
	@param PathToFile	PathToFile	Path to the file to create (including the extension).
	@param FileContent	FileContent	The file's content.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SaveStringArrayToFile", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static bool SaveStringArrayToFile(FString PathToFile, TArray<FString> FileContent);

	/* This function will append the input string array to the file's content. The AppendFileToStringArray param will insert the input content before the file's. 
	@param PathToFile				Path to the file to edit.
	@param FileContent				Content to append to the file.
	@param AppendFileToStringArray	If true, will insert the input FileContent before the file's content.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AppendStringArrayToFile", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static bool AppendStringArrayToFile(FString PathToFile, TArray<FString> FileContent, bool AppendFileToStringArray);

	/***** Path Utilities *****/

	/* This function will return a file extension from the input path. 
	@param PathToFile Path to get the extension from.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetExtension", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static FString GetFileExtension(FString Path);

	/* This function will return a valid directory path from the input path (without a filename nor extension). 
	@param PathToFile The path to extract the valid directory from.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetDirectoryPath", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static FString GetFilePath(FString Path);

	/* This function will return a filename from the input path. 
	@param Path				Path to extract the filename from.
	@param IncludeExtension If true, the filename will be returned with its extension.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetFilename", Keywords = "FileSystemLibrary"), Category = "SystemFile I/O")
	static FString GetFileName(FString Path, bool IncludeExtension);

	/***** File Dialogs *****/
	
	/*This will open a Folder Multi Select dialog. The Directories return value contains the list of paths that were selected.
	@param DialogTitle	Title of the dialog window.
	@param DefaultPath	Path to open by default (default is blank).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OpenFolderMultiSelectDialog", Keywords = "FileSystemLibrary"), Category = "System File Dialogs")
	static bool OpenFolderMultiSelectDialog(TArray<FString> &Directories, FString DialogTitle = "Select a folder", FString DefaultPath = "");

	/*This will open a Folder Select dialog. The FolderPath return value contains the path for the folder selected.
	@param DialogTitle		Title of the dialog window.
	@param DefaultPath		Path to open by default (default is blank).
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OpenFolderSelectDialog", Keywords = "FileSystemLibrary"), Category = "System File Dialogs")
	static bool OpenFolderSelectDialog(FString &FolderPath, FString DialogTitle = "Select a folder", FString DefaultPath = "");
	
	/*This will open a Folder Select dialog that allows multiple files to be selected. The FilePath return value contain the path for the file selected, its name and its extension.
	@param DialogTitle		Title of the dialog window.
	@param DefaultPath		Path to open by default (default is blank).
	@param FileTypes		The file type filter (you can add as many as you need). The format is: [Type Name] (*.[Type Extension]*)|*.[Type Extension]*|
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OpenFileMultiSelectDialog", Keywords = "FileSystemLibrary"), Category = "System File Dialogs")
	static bool OpenFileMultiSelectDialog(TArray<FString> &FilePaths, FString DialogTitle = "Select a file", FString DefaultPath = "", bool AllowMultiSelect = false, FString FileTypes = "All Files (*.*)|*.*|");

	/*This will open a Folder Select dialog. The FilePath return value contain the path for the file selected, its name and its extension.
	@param DialogTitle		Title of the dialog window.
	@param DefaultPath		Path to open by default (default is blank).
	@param FileTypes		The file type filter (you can add as many as you need). The format is: [Type Name] (*.[Type Extension]*)|*.[Type Extension]*|
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OpenFileSelectDialog", Keywords = "FileSystemLibrary"), Category = "System File Dialogs")
	static bool OpenFileSelectDialog(FString &FilePath, FString DialogTitle = "Select a file", FString DefaultPath = "", FString FileTypes = "All Files (*.*)|*.*|");

	/*This will open a File Save dialog. The return value contains the path to the file selected, its name and extension.
	@param DialogTitle		Title of the dialog window.
	@param DefaultPath		Path to open by default (default is blank).
	@param DefaultFileName	Name to give the file by default.
	@param FileTypes		The file type filter (you can add as many as you need). The format is: [Type Name] (*.[Type Extension]*)|*.[Type Extension]*|
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OpenSaveFileDialog", Keywords = "FileSystemLibrary"), Category = "System File Dialogs")
	static bool OpenSaveFileDialog(FString &SaveToPath, FString DialogTitle = "Select a file", FString DefaultPath = "", FString DefaultFileName = "", FString FileTypes = "All Files (*.*)|*.*|");
	
	/***** Process operations *****/
	
	/* Creates a new process and its primary thread. The new process runs the specified executable file in the security context of the calling process.
		@param	PathToExecutable		The path to the executable to run.
		@param	Arguments				Any command line argument to run when executing.
		@param	LaunchDetached			If true, the process will have its own window.
		@param	LaunchedHidden			If true, the new process will be minimized in the task bar
		@param	LaunchReallyHidden		If true, the new process will not have a window or be in the task bar
		@param	PriorityModifier		2 idle, -1 low, 0 normal, 1 high, 2 higher
		@param	UseWorkingDirectory		If true, will use WorkingDirectory to start the executable in instead of its current directory.
		@param	WorkingDirectory		Directory to start the executable in (required UseWorkingDirectory = true).
		@param ProcessID				The created process' ID, use this to track its execution.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CreateProcess", Keywords = "FileSystemLibrary"), Category = "Process")
	static bool CreateProcess(FString PathToExecutable, FString Arguments, bool LaunchDetached, bool LaunchedHidden, bool LaunchReallyHidden, int PriorityModifier, bool UseWorkingDirectory, FString WorkingDirectory, int32& ProcessID);
	
	/* Returns whether or not a specific process is running or not.
		@param ProcessID	The ID of the process to query.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "IsProcessRunning", Keywords = "FileSystemLibrary"), Category = "Process")
	static bool IsProcessRunning(int32 ProcessID);
	
	/* Returns the name of the process.
		@param ProcessID	The ID of the process to query.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "GetProcessName", Keywords = "FileSystemLibrary"), Category = "Process")
	static FString GetProcessName(int32 ProcessID);
};

/***** AsynAction to launch a process and trigger a callback when it finishes. *****/
UCLASS()
class UCreateProcessWithCallback : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompleted);
	UPROPERTY(BlueprintAssignable)
	FOnCompleted Completed;

	/* Same as CreateProcess but has an output pin that executes when the process has ended. 
	Use with caution, this might cause performance issues as we are checking whether the process
	is running every tick.

		@param	PathToExecutable		The path to the executable to run.
		@param	Arguments				Any command line argument to run when executing.
		@param	LaunchDetached			If true, the process will have its own window.
		@param	LaunchedHidden			If true, the new process will be minimized in the task bar
		@param	LaunchReallyHidden		If true, the new process will not have a window or be in the task bar
		@param	PriorityModifier		2 idle, -1 low, 0 normal, 1 high, 2 higher
		@param	UseWorkingDirectory		If true, will use WorkingDirectory to start the executable in instead of its current directory.
		@param	WorkingDirectory		Directory to start the executable in (required UseWorkingDirectory = true).
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName = "CreateProcessWithCallback", Keywords = "process create execute"), Category = "FileSystemLibrary")
	static UCreateProcessWithCallback* CreateProcessWithCallback(UObject* WorldContextObj, FString PathToExecutable, FString Arguments, bool LaunchDetached, bool LaunchedHidden, bool LaunchReallyHidden, int PriorityModifier, bool UseWorkingDirectory, FString WorkingDirectory);

	virtual void Activate() override;

	private:
	UFUNCTION()
	void bIsProcessRunning();


	UObject* WorldContextObj;
	
	uint32 ProcessID;
};
