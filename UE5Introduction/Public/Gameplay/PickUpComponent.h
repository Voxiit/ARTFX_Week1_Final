// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyEnums.h"
#include "PickUpComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickUpDestroyedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUpDestroyedWithTypeDelegate, EPickUpType, PickUpType);

USTRUCT(BlueprintType)
struct FPickUpStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "PickUp")
	EPickUpType PickUpType = EPickUpType::Normal;
	UPROPERTY(EditAnywhere, Category = "PickUp", meta = (EditCondition = "PickUpType != EPickUpType::Normal", EditConditionHides))
	float DestructionTimer = 5.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5INTRODUCTION_API UPickUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPickUpComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditAnywhere, Category = "PickUp")
	FPickUpStruct PickUpStruct;


// Destruction
protected:
	FTimerHandle PickUpDestructionTimerHandle;

public:
	FOnPickUpDestroyedDelegate OnPickUpDestroyed;

public:
	void ClearDestructionTimer();
	void StartPickUpDestructionTimer();
	EPickUpType GetPickUpType() const;

	void DestroyPickUp();

// End of Destruction


// Noted Exercice
public:
	FOnPickUpDestroyedWithTypeDelegate OnPickUpDestroyedWithType;

public:
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

// End of Noted Exerice
};
