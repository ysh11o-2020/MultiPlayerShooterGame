// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG/SSD_MenuWidget.h"
#include "Components/Button.h"

void USSD_MenuWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

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
}

void USSD_MenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this,&USSD_MenuWidget::OnHostButtonClicked);	
	}
	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this,&USSD_MenuWidget::OnJoinButtonClicked);	
	}
}

void USSD_MenuWidget::OnHostButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			0,
			1.0f,
			FColor::Yellow,
			TEXT("Host Button Clicked!!"));
	}
}

void USSD_MenuWidget::OnJoinButtonClicked()
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			0,
			1.0f,
			FColor::Yellow,
			TEXT("Join Button Clicked!!"));
	}
}
