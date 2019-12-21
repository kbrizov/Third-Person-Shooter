// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "TimerManager.h"
#include "MenuWidget.h"
#include "HealthWidget.h"
#include "WeaponWidget.h"
#include "CrosshairWidget.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ABasePlayerController::ABasePlayerController() :
	bIsMenuDisplayed(false),
	MenuWidget(nullptr),
	HealthWidget(nullptr),
	WeaponWidget(nullptr),
	CrosshairWidget(nullptr)
{
	this->SetTeam(ETeam::Player);
}

void ABasePlayerController::ShowMenu()
{
	if (MenuWidget == nullptr)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(this, true);
	this->bIsMenuDisplayed = true;
	this->bShowMouseCursor = true;
	this->MenuWidget->SetVisibility(ESlateVisibility::Visible);

	FInputModeGameAndUI InputMode = FInputModeGameAndUI();
	InputMode.SetWidgetToFocus(this->MenuWidget->TakeWidget());
	this->SetInputMode(InputMode);
}

void ABasePlayerController::HideMenu()
{
	if (MenuWidget == nullptr)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(this, false);
	this->bIsMenuDisplayed = false;
	this->bShowMouseCursor = false;
	this->MenuWidget->SetVisibility(ESlateVisibility::Hidden);

	FInputModeGameOnly InputMode = FInputModeGameOnly();
	this->SetInputMode(InputMode);
}

ETeam ABasePlayerController::GetTeam() const
{
	return this->Team;
}

void ABasePlayerController::SetTeam(ETeam NewTeam)
{
	this->Team = NewTeam;
}

FGenericTeamId ABasePlayerController::GetGenericTeamId() const
{
	return this->TeamId;
}

void ABasePlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (TeamId != NewTeamId)
	{
		this->TeamId = NewTeamId;
	}
}

void ABasePlayerController::SetGenericTeamId(ETeam NewTeam)
{
	uint8 Id = static_cast<uint8>(NewTeam);
	this->SetGenericTeamId(FGenericTeamId(Id));
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	this->SetGenericTeamId(this->Team);
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this->GetCharacter());

	if (PlayerCharacter)
	{
		PlayerCharacter->DeathEvent.AddUObject(this, &ABasePlayerController::OnPlayerDeath);
	}

	this->AddPlayerUIWidgetsToViewport();

	if (MenuWidgetClass)
	{
		this->MenuWidget = CreateWidget<UMenuWidget>(this, MenuWidgetClass);
		this->MenuWidget->AddToViewport();
		this->MenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	this->InputComponent->BindAction(this->ToggleMenuActionName, EInputEvent::IE_Pressed, this, &ABasePlayerController::OnToggleMenu).bExecuteWhenPaused = true;
}

void ABasePlayerController::OnToggleMenu()
{
	if (!bIsMenuDisplayed)
	{
		this->ShowMenu();
	}
	else
	{
		this->HideMenu();
	}
}

void ABasePlayerController::OnPlayerDeath()
{
	FTimerHandle TimerHandle;
	this->GetWorldTimerManager().SetTimer(TimerHandle, this, &ABasePlayerController::ShowMenu, 2.0f, false);
}

void ABasePlayerController::AddPlayerUIWidgetsToViewport()
{
	if (HealthWidgetClass)
	{
		this->HealthWidget = CreateWidget<UHealthWidget>(this, HealthWidgetClass);
		this->HealthWidget->AddToViewport();
	}

	if (WeaponWidgetClass)
	{
		this->WeaponWidget = CreateWidget<UWeaponWidget>(this, WeaponWidgetClass);
		this->WeaponWidget->AddToViewport();
	}

	if (CrosshairWidgetClass)
	{
		this->CrosshairWidget = CreateWidget<UCrosshairWidget>(this, CrosshairWidgetClass);
		this->CrosshairWidget->AddToViewport();
	}
}

void ABasePlayerController::RemovePlayerUIWidgetsFromViewport()
{
	if (HealthWidget)
	{
		this->HealthWidget->RemoveFromViewport();
	}

	if (WeaponWidget)
	{
		this->WeaponWidget->RemoveFromViewport();
	}

	if (CrosshairWidget)
	{
		this->CrosshairWidget->RemoveFromViewport();
	}
}
