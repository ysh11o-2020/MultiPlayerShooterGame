// Copyright Epic Games, Inc. All Rights Reserved.

#include "SteamSessionDemoCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

//////////////////////////////////////////////////////////////////////////
// ASteamSessionDemoCharacter

ASteamSessionDemoCharacter::ASteamSessionDemoCharacter():
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ASteamSessionDemoCharacter::OnCreateGameSessionCompleted)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this,&ASteamSessionDemoCharacter::OnFindGameSessionCompleted)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this,&ASteamSessionDemoCharacter::OnJoinGameSessionCompleted))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				0,
				15.0f,
				FColor::Green,
				FString::Printf(TEXT("Found Subsystem %s"),*OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
}

void ASteamSessionDemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASteamSessionDemoCharacter::CreateGameSession()
{
	if(!OnlineSessionInterface.IsValid())
	{
		return;
	}
	
	FNamedOnlineSession* ExistingGameSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingGameSession!=nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->Set(FName("MatchType"),FString("FreeForAll"),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession,*SessionSettings);
}

void ASteamSessionDemoCharacter::JoinGameSession()
{
	if(!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch());
	OnlineSessionSearch->bIsLanQuery = false;
	OnlineSessionSearch->MaxSearchResults = 10000;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(),OnlineSessionSearch.ToSharedRef());
}

void ASteamSessionDemoCharacter::OnCreateGameSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				0,
				2.0f,
				bWasSuccessful?FColor::Green:FColor::Red,
				FString::Printf(TEXT("Create session %s %s"),*SessionName.ToString(),bWasSuccessful?TEXT("succeed!"):TEXT("failed.")));
		}

		UWorld* World = GetWorld();
		if(World)
		{
			World->ServerTravel(FString("/Game/Map/LobbyMap?listen"));
		}
	}
	
}

void ASteamSessionDemoCharacter::OnFindGameSessionCompleted(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		for(FOnlineSessionSearchResult Result:OnlineSessionSearch->SearchResults)
		{
			FString SessionID = Result.GetSessionIdStr();
			FString SessionOwner = Result.Session.OwningUserName;
			FString MatchType;
			Result.Session.SessionSettings.Get(FName("MatchType"),MatchType);
			
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					0,
					2.0f,
					FColor::Orange,
					FString::Printf(TEXT("Id: %s , User: %s"),*SessionID,*SessionOwner)
				);
			}

			if(MatchType == FString("FreeForAll"))
			{
				if(GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						0,
						15.0f,
						FColor::Cyan,
						FString::Printf(TEXT("Joining Match Type %s"),*MatchType)
					);
				}

				OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
				const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession,Result);
			}
		}
	}
}

void ASteamSessionDemoCharacter::OnJoinGameSessionCompleted(FName SessionName,
	EOnJoinSessionCompleteResult::Type Result)
{
	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				0,
				5.0f,
				FColor::Green,
				FString::Printf(TEXT("Join Sesssion %s Succeed!"),*SessionName.ToString())
			);
		}
	}

	FString Address;
	if(OnlineSessionInterface && OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession,Address))
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				0,
				5.0f,
				FColor::Green,
				FString::Printf(TEXT("Connect string: %s"),*Address)
			);
		}
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if(PlayerController)
		{
			PlayerController->ClientTravel(Address,ETravelType::TRAVEL_Absolute);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASteamSessionDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASteamSessionDemoCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASteamSessionDemoCharacter::Look);

		//Game Session Creating
		EnhancedInputComponent->BindAction(HostAction,ETriggerEvent::Started,this,&ASteamSessionDemoCharacter::CreateGameSession);

		//Game Session Finding
		EnhancedInputComponent->BindAction(FindSessionAction,ETriggerEvent::Started,this,&ASteamSessionDemoCharacter::JoinGameSession);
	}

}

void ASteamSessionDemoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASteamSessionDemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




