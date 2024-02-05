// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "MultiplayerSessionSubsystem.h"
#include "Components/Button.h"


void UMenuWidget::MenuSetup(int32 MaxConnectionsNum, FString MatchTypeStr)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	MaxConnections = MaxConnectionsNum;
	MatchType = MatchTypeStr;
	
	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if(PC)
		{
			FInputModeUIOnly InputModeUIOnly;
			InputModeUIOnly.SetWidgetToFocus(TakeWidget());
			InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputModeUIOnly);
			PC->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GI = GetGameInstance();
	if(GI)
	{
		MultiplayerSessionSubsystem = GI->GetSubsystem<UMultiplayerSessionSubsystem>();
	}

	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerCreateSessionCompleted.AddDynamic(this,&UMenuWidget::OnCreateSessionComplete);
	}
}

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this,&UMenuWidget::OnHostButtonClicked);
	}
	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this,&UMenuWidget::OnJoinButtonClicked);
	}
}

void UMenuWidget::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenuWidget::OnHostButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			0,
			1.0f,
			FColor::Orange,
			TEXT("Host button is clicked")
			);
	}

	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(MaxConnections,MatchType);
		UWorld* World = GetWorld();
		if(World)
		{
			World->ServerTravel(FString("/Game/Map/LobbyMap?listen"));
		}
	}
}

void UMenuWidget::OnJoinButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			0,
			1.0f,
			FColor::Orange,
			TEXT("Join button is clicked")
		);
	}
}

void UMenuWidget::OnCreateSessionComplete_Implementation(bool bWasSuccessful)
{
	//Implement in blueprint
}

void UMenuWidget::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if(PC)
		{
			FInputModeGameOnly InputData;
			PC->SetInputMode(InputData);
			PC->SetShowMouseCursor(true);
		}
	}
}
