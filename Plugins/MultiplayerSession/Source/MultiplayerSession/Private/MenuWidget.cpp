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
		MultiplayerSessionSubsystem->MultiplayerFindSessionCompleted.AddUObject(this,&UMenuWidget::OnFindSessionComplete);
		MultiplayerSessionSubsystem->MultiplayerJoinSessionCompleted.AddUObject(this,&UMenuWidget::OnJoinSessionComplete);
		MultiplayerSessionSubsystem->MultiplayerDestroySessionCompleted.AddDynamic(this,&UMenuWidget::OnDestroySessionComplete);
		MultiplayerSessionSubsystem->MultiplayerStartSessionCompleted.AddDynamic(this,&UMenuWidget::OnStartSessionCompltet);
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
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSession(10000);
	}
}

void UMenuWidget::OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if(MultiplayerSessionSubsystem == nullptr)
	{
		return;
	}
	
	for(FOnlineSessionSearchResult SessionResult:SessionResults)
	{
		FString SessionValue;
		SessionResult.Session.SessionSettings.Get(FName("MatchType"),SessionValue);
		if(SessionValue == MatchType)
		{
			MultiplayerSessionSubsystem->JoinSession(SessionResult);
			//find one room & join(for template)
			return;
		}
	}
}

void UMenuWidget::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession,Address);
			APlayerController* MyPC = GetGameInstance()->GetFirstLocalPlayerController();
			if(MyPC)
			{
				MyPC->ClientTravel(Address,TRAVEL_Absolute);
			}
		}
	}
}

void UMenuWidget::OnDestroySessionComplete_Implementation(bool bWasSuccessful)
{
}

void UMenuWidget::OnCreateSessionComplete_Implementation(bool bWasSuccessful)
{
	UWorld* World = GetWorld();
	if(World && bWasSuccessful)
	{
		World->ServerTravel(FString("/Game/Map/LobbyMap?listen"));
	}
	//Implement in blueprint
}

void UMenuWidget::OnStartSessionCompltet_Implementation(bool bWasSuccessful)
{
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
