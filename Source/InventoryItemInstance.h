// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "UObject/Object.h"
#include "ItemModifierInterface.h"
#include "InventoryItemInstance.generated.h"

USTRUCT(BlueprintType)
struct FItemDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FText ItemName;

	// Referencia suave: NO carga la malla a memoria al inicializar la estructura
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	TSoftObjectPtr<UStaticMesh> VisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	float PowerValue = 10.f;
};

 // Objeto que representa la instancia del ítem en memoria
UCLASS(BlueprintType)
class MODULARINVENTORY_API UInventoryItemInstance : public UObject, public IItemModifierInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Item")
	FItemDefinition Data;

	// Heredado vía IItemModifierInterface
	virtual void ApplyModifier(AActor* TargetActor) override;
};
