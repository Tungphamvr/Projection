// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "Android/DialogManagerAndroid.h"
#include "FileSystemLibraryLog.h"
#include "FileSystemLibrary.h"

#include "Misc/Paths.h"
#include "Misc/Guid.h"
#include "HAL/FileManager.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidPlatform.h"
#include "Android/AndroidJava.h"
#include "Android/AndroidJavaEnv.h"
#include "Android/AndroidApplication.h"

#if USE_ANDROID_JNI
#include <jni.h>
#endif
#endif


bool DialogManagerAndroid::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames)
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("Failed to get Java Env"));
		return false;
	}

	//public void AndroidThunkJava_fflOpenFileDialog(Uri defaultPath, String[] mimeTypes, boolean multiSelect) 
    static jmethodID AndroidThunkJava_fflOpenFileDialog = FJavaWrapper::FindMethod(
        Env,
        FJavaWrapper::GameActivityClassID,
        "AndroidThunkJava_fflOpenFileDialog",
        "(Landroid/net/Uri;[Ljava/lang/String;Z)V",
        false
    );
    checkf(AndroidThunkJava_fflOpenFileDialog != nullptr, TEXT("AndroidThunkJava_fflOpenFileDialog jmethod is null."));
	
	jobject jUriDefaultPath = nullptr; // @TODO: Needs custom DefaultPath to URI function
    jboolean jMultiSelect = static_cast<jboolean>(bMultiSelect);
	
	TArray<FString> MimeTypes = ConvertFileTypesToMimeTypes(FileTypes);
	jobjectArray jMimeTypes = Env->NewObjectArray(MimeTypes.Num(), FJavaWrapper::JavaStringClass, nullptr);
	for (int i = 0; i < MimeTypes.Num(); i++)
	{
		jstring jMime = Env->NewStringUTF(TCHAR_TO_UTF8(*MimeTypes[i]));
		Env->SetObjectArrayElement(jMimeTypes, i, jMime);
		Env->DeleteLocalRef(jMime);
	}
	
	FJavaWrapper::CallVoidMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		AndroidThunkJava_fflOpenFileDialog,
		jUriDefaultPath,
		jMimeTypes,
		jMultiSelect
	);

	Env->DeleteLocalRef(jUriDefaultPath);
	Env->DeleteLocalRef(jMimeTypes);
	
	return true;
#endif

	return false;
}

bool DialogManagerAndroid::SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames)
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("Failed to get Java Env"));
		return false;
	}

	//public void AndroidThunkJava_fflSaveFileDialog(String defaultFile, Uri defaultPath, String[] mimeTypes, boolean multiSelect) 
	static jmethodID AndroidThunkJava_fflSaveFileDialog = FJavaWrapper::FindMethod(
		Env,
		FJavaWrapper::GameActivityClassID,
		"AndroidThunkJava_fflSaveFileDialog",
		"(Ljava/lang/String;Landroid/net/Uri;[Ljava/lang/String;Z)V",
		false
	);
	checkf(AndroidThunkJava_fflSaveFileDialog != nullptr, TEXT("AndroidThunkJava_fflSaveFileDialog jmethod is null."));

	
	jstring jDefaultFile  = Env->NewStringUTF(TCHAR_TO_UTF8(*DefaultFile));
	jobject jUriDefaultPath = nullptr; // @TODO: Needs custom DefaultPath to URI function
	jboolean jMultiSelect = static_cast<jboolean>(bMultiSelect);
	
	TArray<FString> MimeTypes = ConvertFileTypesToMimeTypes(FileTypes);
	jobjectArray jMimeTypes = Env->NewObjectArray(MimeTypes.Num(), FJavaWrapper::JavaStringClass, nullptr);
	for (int i = 0; i < MimeTypes.Num(); i++)
	{
		jstring jMime = Env->NewStringUTF(TCHAR_TO_UTF8(*MimeTypes[i]));
		Env->SetObjectArrayElement(jMimeTypes, i, jMime);
		Env->DeleteLocalRef(jMime);
	}
	
	FJavaWrapper::CallVoidMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		AndroidThunkJava_fflSaveFileDialog,
		jDefaultFile,
		jUriDefaultPath,
		jMimeTypes,
		jMultiSelect
	);

	Env->DeleteLocalRef(jDefaultFile);
	Env->DeleteLocalRef(jUriDefaultPath);
	Env->DeleteLocalRef(jMimeTypes);

	return true;
#endif

	return false;
}

bool DialogManagerAndroid::OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, bool bMultiSelect, TArray<FString>& OutDirectories)
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env)
	{
		UE_LOG(FileSystemLibraryLog, Error, TEXT("Failed to get Java Env"));
		return false;
	}

	//public void AndroidThunkJava_fflOpenDirectoryDialog(Uri defaultPath, boolean multiSelect)
	static jmethodID AndroidThunkJava_fflOpenDirectoryDialog = FJavaWrapper::FindMethod(
		Env,
		FJavaWrapper::GameActivityClassID,
		"AndroidThunkJava_fflOpenDirectoryDialog",
		"(Landroid/net/Uri;Z)V",
		false
	);
	checkf(AndroidThunkJava_fflOpenDirectoryDialog != nullptr, TEXT("AndroidThunkJava_fflOpenDirectoryDialog jmethod is null."));

	
	jobject jUriDefaultPath = nullptr; // @TODO: Needs custom DefaultPath to URI function
	jboolean jMultiSelect = static_cast<jboolean>(bMultiSelect);
	
	FJavaWrapper::CallVoidMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		AndroidThunkJava_fflOpenDirectoryDialog,
		jUriDefaultPath,
		jMultiSelect
	);

	Env->DeleteLocalRef(jUriDefaultPath);
	
	return true;
#endif

	return false;
}


/* ===== Utils ===== */
TArray<FString> DialogManagerAndroid::ConvertFileTypesToMimeTypes(const FString& FileTypes)
{
	TArray<FString> Result;

	// "WAV (* .wav)|*.wav| MP3 (*.MP3)|*.mp3"  ->  "WAV (* .wav) ; *.wav ;  MP3 (*.MP3) ; *.mp3" 
	TArray<FString> Tokens;
	FileTypes.ParseIntoArray(Tokens, TEXT("|"), true);
	
	for (int32 i = 0; i < Tokens.Num(); i++)
	{
		if (Tokens[i].Contains(TEXT("*."), ESearchCase::IgnoreCase))
		{
			FString Mime = GetMimeTypeFromFilePath(Tokens[i]);
			Result.Add(Mime);
		}
	}

	if (Result.Num() == 0)
	{
		Result.Add(TEXT("*/*"));
	}

	return Result;
}

FString DialogManagerAndroid::GetMimeTypeFromFilePath(const FString& FilePath)
{
	const FString FileExtension = FPaths::GetExtension(FilePath, true);

	static TMap<FString, FString> ExtensionMimeTypeMap;
	if ( ExtensionMimeTypeMap.Num() == 0 )
	{
	    // Web
	    ExtensionMimeTypeMap.Add(TEXT(".html"), TEXT("text/html"));
	    ExtensionMimeTypeMap.Add(TEXT(".css"), TEXT("text/css"));
	    ExtensionMimeTypeMap.Add(TEXT(".js"), TEXT("application/x-javascript"));

	    // Video
	    ExtensionMimeTypeMap.Add(TEXT(".avi"), TEXT("video/msvideo, video/avi, video/x-msvideo"));
	    ExtensionMimeTypeMap.Add(TEXT(".mpeg"), TEXT("video/mpeg"));

	    // Image
	    ExtensionMimeTypeMap.Add(TEXT(".bmp"), TEXT("image/bmp"));
	    ExtensionMimeTypeMap.Add(TEXT(".gif"), TEXT("image/gif"));
	    ExtensionMimeTypeMap.Add(TEXT(".jpg"), TEXT("image/jpeg"));
	    ExtensionMimeTypeMap.Add(TEXT(".jpeg"), TEXT("image/jpeg"));
	    ExtensionMimeTypeMap.Add(TEXT(".png"), TEXT("image/png"));
	    ExtensionMimeTypeMap.Add(TEXT(".svg"), TEXT("image/svg+xml"));
	    ExtensionMimeTypeMap.Add(TEXT(".tiff"), TEXT("image/tiff"));

	    // Audio
	    ExtensionMimeTypeMap.Add(TEXT(".midi"), TEXT("audio/x-midi"));
	    ExtensionMimeTypeMap.Add(TEXT(".mp3"), TEXT("audio/mpeg"));
	    ExtensionMimeTypeMap.Add(TEXT(".ogg"), TEXT("audio/vorbis, application/ogg"));
	    ExtensionMimeTypeMap.Add(TEXT(".wav"), TEXT("audio/wav, audio/x-wav"));

	    // Documents
	    ExtensionMimeTypeMap.Add(TEXT(".xml"), TEXT("application/xml"));
	    ExtensionMimeTypeMap.Add(TEXT(".txt"), TEXT("text/plain"));
	    ExtensionMimeTypeMap.Add(TEXT(".tsv"), TEXT("text/tab-separated-values"));
	    ExtensionMimeTypeMap.Add(TEXT(".csv"), TEXT("text/csv"));
	    ExtensionMimeTypeMap.Add(TEXT(".json"), TEXT("application/json"));
	    ExtensionMimeTypeMap.Add(TEXT(".pdf"), TEXT("application/pdf"));
	    ExtensionMimeTypeMap.Add(TEXT(".doc"), TEXT("application/msword"));
	    ExtensionMimeTypeMap.Add(TEXT(".docx"), TEXT("application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	    ExtensionMimeTypeMap.Add(TEXT(".xls"), TEXT("application/vnd.ms-excel"));
	    ExtensionMimeTypeMap.Add(TEXT(".xlsx"), TEXT("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	    ExtensionMimeTypeMap.Add(TEXT(".ppt"), TEXT("application/vnd.ms-powerpoint"));
	    ExtensionMimeTypeMap.Add(TEXT(".pptx"), TEXT("application/vnd.openxmlformats-officedocument.presentationml.presentation"));

	    // Compressed
	    ExtensionMimeTypeMap.Add(TEXT(".zip"), TEXT("application/zip, application/x-compressed-zip"));
	    ExtensionMimeTypeMap.Add(TEXT(".7z"), TEXT("application/x-7z-compressed"));
	    ExtensionMimeTypeMap.Add(TEXT(".rar"), TEXT("application/x-rar-compressed"));
	    
	    // Android package
	    ExtensionMimeTypeMap.Add(TEXT(".apk"), TEXT("application/vnd.android.package-archive"));
	    
	    // Others
	    ExtensionMimeTypeMap.Add(TEXT(".bin"), TEXT("application/octet-stream")); // Common fallback for unknown binary data
	    ExtensionMimeTypeMap.Add(TEXT(".exe"), TEXT("application/x-msdownload")); // Common usage for Windows executables
	    ExtensionMimeTypeMap.Add(TEXT(".epub"), TEXT("application/epub+zip"));
	}

	if ( FString* FoundMimeType = ExtensionMimeTypeMap.Find(FileExtension) )
	{
		return *FoundMimeType;
	}

	return TEXT("*/*");
}


/* ==== Callbacks ==== */
void DialogManagerAndroid::HandleDialogResults(const TArray<FString>& Paths)
{
	for (FString Path : Paths)
	{
		UE_LOG(FileSystemLibraryLog, Warning, TEXT("DialogResult multi : %s"), *Path);
	}

	
	Results = Paths;
	bRecievedResults = true;
}

void DialogManagerAndroid::ResetResults()
{
	bRecievedResults = false;
	Results.Empty();
}

#if USE_ANDROID_JNI
// This function is declared in FileSystemLibrary_UPL.xml: public native void ffl_handleondialogresult(String path);
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_fflHandleOnDialogResult(JNIEnv* jenv, jobject thiz, jstring path)
{
	if (!path)
	{
		return;
	}

	const char* nativeString = jenv->GetStringUTFChars(path, 0);
	TArray<FString> ReturnStrings;
	ReturnStrings.Add(UTF8_TO_TCHAR(nativeString));
	jenv->ReleaseStringUTFChars(path, nativeString);
    
	// Dispatch to your existing single-result handler
	static_cast<DialogManagerAndroid*>(FFileSystemLibraryModule::GetDialogManager())->HandleDialogResults(ReturnStrings);
}

// This function is declared in FileSystemLibrary_UPL.xml: public native void ffl_handleondialogresultMultiple(String[] paths);
JNI_METHOD void Java_com_epicgames_unreal_GameActivity_fflHandleOnDialogResultMulti(JNIEnv* jenv, jobject thiz,	jobjectArray paths)
{
	if (!paths)
	{
		return;
	}

	jsize arrayLength = jenv->GetArrayLength(paths);
	TArray<FString> ReturnStrings;
	ReturnStrings.Reserve(arrayLength);

	for (jsize i = 0; i < arrayLength; i++)
	{
		// Retrieve each String in the object array
		jstring jPath = (jstring) jenv->GetObjectArrayElement(paths, i);
		if (jPath != nullptr)
		{
			const char* nativeString = jenv->GetStringUTFChars(jPath, 0);
			FString PathString(UTF8_TO_TCHAR(nativeString));

			jenv->ReleaseStringUTFChars(jPath, nativeString);
			jenv->DeleteLocalRef(jPath);

			ReturnStrings.Add(PathString);
		}
	}


	// Dispatch to your existing multi-results handler
	static_cast<DialogManagerAndroid*>(FFileSystemLibraryModule::GetDialogManager())->HandleDialogResults(ReturnStrings);
}
#endif
