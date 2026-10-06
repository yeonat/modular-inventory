// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InventoryComponent.h"
#include "TestPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MODULARINVENTORY_API ATestPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Comando: AddItem <ID>
	// Ejemplo en consola: AddItem 1
	UFUNCTION(Exec)
	void AddItem(int32 ItemID);

	// Comando: EquipItem <ID>
	// Ejemplo en consola: EquipItem 0
	UFUNCTION(Exec)
	void EquipItem(int32 ItemIndex);

	// Comando: TestGC
	UFUNCTION(Exec)
	void TestGC();

private:
	UInventoryComponent* GetInventory() const;
	UStaticMeshComponent* GetTargetMeshComponent() const;
	FItemDefinition CreateDummyItem(int32 ItemID);
}; 


