// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Environment/DoubleWingDoor.h"
#include <Components/BoxComponent.h>
#include <Components/AudioComponent.h>
#include <Components/StaticMeshComponent.h>

const FName ADoubleWingDoor::AudioComponentName = FName(TEXT("DoorAudio"));
const FName ADoubleWingDoor::FrameComponentName = FName(TEXT("Frame"));
const FName ADoubleWingDoor::LeftWingComponentName = FName(TEXT("LeftWing"));
const FName ADoubleWingDoor::RightWingComponentName = FName(TEXT("RightWing"));
const FName ADoubleWingDoor::TriggerVolumeComponentName = FName(TEXT("TriggerVolume"));

ADoubleWingDoor::ADoubleWingDoor(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	bIsOpening(false),
	bIsClosing(false),
	LeftWingClosedLocation(FVector::ZeroVector),
	LeftWingOpenedLocation(FVector::ZeroVector),
	RightWingClosedLocation(FVector::ZeroVector),
	RightWingOpenedLocation(FVector::ZeroVector),
	Alpha(0.025f),
	WingOffset(160.0f),
	OpeningSound(nullptr),
	ClosingSound(nullptr)
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->Frame = this->CreateDefaultSubobject<UStaticMeshComponent>(ADoubleWingDoor::FrameComponentName);
	this->RootComponent = this->Frame;

	this->LeftWing = this->CreateDefaultSubobject<UStaticMeshComponent>(ADoubleWingDoor::LeftWingComponentName);
	this->LeftWing->SetupAttachment(this->RootComponent);

	this->RightWing = this->CreateDefaultSubobject<UStaticMeshComponent>(ADoubleWingDoor::RightWingComponentName);
	this->RightWing->SetupAttachment(this->RootComponent);

	this->TriggerVolume = this->CreateDefaultSubobject<UBoxComponent>(ADoubleWingDoor::TriggerVolumeComponentName);
	this->TriggerVolume->SetupAttachment(this->RootComponent);

	this->AudioComponent = this->CreateDefaultSubobject<UAudioComponent>(ADoubleWingDoor::AudioComponentName);
	this->AudioComponent->SetupAttachment(this->RootComponent);
}

void ADoubleWingDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsOpening)
	{
		this->SlideDoorWing(this->LeftWing, this->LeftWingOpenedLocation);
		this->SlideDoorWing(this->RightWing, this->RightWingOpenedLocation);

		if (IsOpen())
		{
			bIsOpening = false;
		}
	}
	else if (bIsClosing)
	{
		this->SlideDoorWing(this->LeftWing, this->LeftWingClosedLocation);
		this->SlideDoorWing(this->RightWing, this->RightWingClosedLocation);

		if (IsClosed())
		{
			bIsClosing = false;
		}
	}
}

void ADoubleWingDoor::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(this->LeftWing != nullptr, TEXT("LeftWingComponent is null."));
	checkf(this->RightWing != nullptr, TEXT("RightWingComponent is null."));
	
	const FTransform LeftWingTransform = this->LeftWing->GetRelativeTransform();
	const float LeftWingYScale = LeftWingTransform.GetScale3D().Y;
	const float LeftWingDelta = this->WingOffset * LeftWingYScale;
	this->LeftWingClosedLocation = LeftWingTransform.GetLocation();
	this->LeftWingOpenedLocation = this->LeftWingClosedLocation;
	this->LeftWingOpenedLocation.Y = this->LeftWingClosedLocation.Y - LeftWingDelta;

	const FTransform RightWingTransform = this->RightWing->GetRelativeTransform();
	const float RightWingYScale = RightWingTransform.GetScale3D().Y;
	const float RightWingDelta = this->WingOffset * RightWingYScale;
	this->RightWingClosedLocation = RightWingTransform.GetLocation();
	this->RightWingOpenedLocation = this->RightWingClosedLocation;
	this->RightWingOpenedLocation.Y = this->RightWingClosedLocation.Y + RightWingDelta;

	this->TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoubleWingDoor::OnDoorTriggerBeginOverlap);
	this->TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ADoubleWingDoor::OnDoorTriggerEndOverlap);
}

void ADoubleWingDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TriggerVolume)
	{
		this->TriggerVolume->OnComponentBeginOverlap.RemoveDynamic(this, &ADoubleWingDoor::OnDoorTriggerBeginOverlap);
		this->TriggerVolume->OnComponentEndOverlap.RemoveDynamic(this, &ADoubleWingDoor::OnDoorTriggerEndOverlap);
	}

	Super::EndPlay(EndPlayReason);
}

void ADoubleWingDoor::OnDoorTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	this->bIsOpening = true;
	this->bIsClosing = false;

	if (AudioComponent)
	{
		if (AudioComponent->IsPlaying())
		{
			this->AudioComponent->Stop();
		}
		
		this->AudioComponent->SetSound(OpeningSound);
		this->AudioComponent->Play();
	}
}

void ADoubleWingDoor::OnDoorTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	this->bIsOpening = false;
	this->bIsClosing = true;

	if (AudioComponent)
	{
		if (AudioComponent->IsPlaying())
		{
			this->AudioComponent->Stop();
		}

		this->AudioComponent->SetSound(ClosingSound);
		this->AudioComponent->Play();
	}
}

void ADoubleWingDoor::SlideDoorWing(UStaticMeshComponent* DoorWing, const FVector& GoalLocation)
{
	checkf(DoorWing != nullptr, TEXT("DoorWing is null."));

	const FVector CurrentWingLocation = DoorWing->GetRelativeTransform().GetLocation();
	const FVector NewWingLocation = FMath::Lerp(CurrentWingLocation, GoalLocation, this->Alpha);
	DoorWing->SetRelativeLocation(NewWingLocation);
}

bool ADoubleWingDoor::IsOpen() const
{
	checkf(this->LeftWing != nullptr, TEXT("LeftWingComponent is null."));
	checkf(this->RightWing != nullptr, TEXT("RightWingComponent is null."));

	const FVector CurrentLeftWingLocation = this->LeftWing->GetComponentLocation();
	const FVector CurrentRightWingLocation = this->RightWing->GetComponentLocation();

	const bool bIsLeftWingOpen = CurrentLeftWingLocation.Equals(this->LeftWingOpenedLocation);
	const bool bIsRightWingOpen = CurrentRightWingLocation.Equals(this->RightWingOpenedLocation);

	const bool bIsOpen = bIsLeftWingOpen && bIsRightWingOpen;

	return bIsOpen;
}

bool ADoubleWingDoor::IsClosed() const
{
	checkf(this->LeftWing != nullptr, TEXT("LeftWingComponent is null."));
	checkf(this->RightWing != nullptr, TEXT("RightWingComponent is null."));

	const FVector CurrentLeftWingLocation = this->LeftWing->GetComponentLocation();
	const FVector CurrentRightWingLocation = this->RightWing->GetComponentLocation();

	const bool bIsLeftWingClosed = CurrentLeftWingLocation.Equals(this->LeftWingClosedLocation);
	const bool bIsRightWingClosed = CurrentRightWingLocation.Equals(this->RightWingClosedLocation);

	const bool bIsClosed = bIsLeftWingClosed && bIsRightWingClosed;

	return bIsClosed;
}
