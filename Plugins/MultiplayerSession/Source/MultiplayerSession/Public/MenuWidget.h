// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

class UMultiplayerSessionSubsystem;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSION_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 MaxConnectionsNum=4,FString MatchTypeStr=TEXT("FreeForAll"));

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void OnCreateSessionComplete(bool bWasSuccessful);
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta=(BindWidget))
	class UButton* JoinButton;

	UPROPERTY()
	UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;

	void MenuTearDown();

	int32 MaxConnections{4};
	FString MatchType{TEXT("FreeForAll")};
};
