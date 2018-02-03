// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamAgentInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UMenuWidget;
class UHealthWidget;
class UWeaponWidget;
class UCrosshairWidget;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API ABasePlayerController : public APlayerController, public ITeamAgentInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	ABasePlayerController();

	UFUNCTION(BlueprintCallable, Category = "User Interface")
	void ShowMenu();

	UFUNCTION(BlueprintCallable, Category = "User Interface")
	void HideMenu();

	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual ETeam GetTeam() const override; // ITeamAgentInterface

	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual void SetTeam(ETeam NewTeam) override; // ITeamAgentInterface

	virtual FGenericTeamId GetGenericTeamId() const override; // IGenericTeamAgentInterface

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override; // IGenericTeamAgentInterface

	virtual void SetGenericTeamId(ETeam NewTeam);

protected:

	const FName ToggleMenuActionName = FName(TEXT("ToggleMenu"));

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:

	bool bIsMenuDisplayed;

	FGenericTeamId TeamId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess = "true"))
	ETeam Team;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UMenuWidget> MenuWidgetClass;

	UPROPERTY()
	UMenuWidget* MenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UHealthWidget> HealthWidgetClass;

	UPROPERTY()
	UHealthWidget* HealthWidget;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UWeaponWidget> WeaponWidgetClass;

	UPROPERTY()
	UWeaponWidget* WeaponWidget;

	UPROPERTY(EditDefaultsOnly, Category = "User Interface")
	TSubclassOf<UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	UCrosshairWidget* CrosshairWidget;

	void OnToggleMenu();

	void OnPlayerDeath();

	void AddPlayerUIWidgetsToViewport();

	void RemovePlayerUIWidgetsFromViewport();
};
