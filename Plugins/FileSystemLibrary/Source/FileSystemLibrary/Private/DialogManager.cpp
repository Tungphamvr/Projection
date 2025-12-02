// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.


#include "DialogManager.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SWindow.h"

#if WITH_EDITOR
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#endif


DialogManager::DialogManager()
{
}

DialogManager::~DialogManager()
{
}

bool DialogManager::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames)
{
	return false;
}

bool DialogManager::SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, bool bMultiSelect, TArray<FString>& OutFilenames)
{
	return false;
}

bool DialogManager::OpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, bool MultiSelect, TArray<FString>& OutDirectories)
{
	return false;
}

void* DialogManager::GetOSWindowHandle() const
{
	void *ParentWindowHandle = nullptr;
	if (!GEngine)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogManager::GetOSWindowHandle(): GEngine is NULL"));
		return nullptr;
	}

	// Get window handle from GameViewport
	if (GEngine->GameViewport)
	{
		ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	}

	// Get Window handle from Editor MainFrame
#if WITH_EDITOR
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();

	if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid())
	{
		ParentWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
	}
#endif

	return ParentWindowHandle;
}

