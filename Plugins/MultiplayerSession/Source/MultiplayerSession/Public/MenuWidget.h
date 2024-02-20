// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	UFUNCTION(BlueprintNativeEvent)
	void OnCreateSessionComplete(bool bWasSuccessful);
	
	void OnFindSessionComplete(const TArray<FOnlineSessionSearchResult>& SessionResults,bool bWasSuccessful);
	
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintNativeEvent)
	void OnDestroySessionComplete(bool bWasSuccessful);

	UFUNCTION(BlueprintNativeEvent)
	void OnStartSessionCompltet(bool bWasSuccessful);
	
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
