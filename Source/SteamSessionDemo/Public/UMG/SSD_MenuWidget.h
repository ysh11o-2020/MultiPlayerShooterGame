// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SSD_MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class STEAMSESSIONDEMO_API USSD_MenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnHostButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();
	
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta=(BindWidget))
	class UButton* JoinButton;
	
};
