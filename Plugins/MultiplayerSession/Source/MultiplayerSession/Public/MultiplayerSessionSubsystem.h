// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerCreateSessionCompleted,bool,bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerFindSessionCompleted,const TArray<FOnlineSessionSearchResult>& SessionResults,bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerJoinSessionCompleted,EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerDestroySessionCompleted,bool,bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerStartSessionCompleted,bool,bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSION_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionSubsystem();

	/** Call Function */
	void CreateSession(int32 NumOfPublicConnection,FString MatchType);
	void FindSession(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	/** Delegates for Menu class to bind Callbacks to */
	FMultiplayerCreateSessionCompleted MultiplayerCreateSessionCompleted;
	FMultiplayerFindSessionCompleted MultiplayerFindSessionCompleted;
	FMultiplayerJoinSessionCompleted MultiplayerJoinSessionCompleted;
	FMultiplayerDestroySessionCompleted MultiplayerDestroySessionCompleted;
	FMultiplayerStartSessionCompleted MultiplayerStartSessionCompleted;
	
protected:
	/** Callback Function */
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName,bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName,bool bWasSuccessful);
	
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	/** Delegate */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
};
