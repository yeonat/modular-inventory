// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemModifierInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemModifierInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MODULARINVENTORY_API IItemModifierInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Método virtual puro que cualquier ítem modificador debe implementar
	virtual void ApplyModifier(class AActor* TargetActor) = 0;

	// = 0 la convierte en una función virtual pura, lo que significa que cualquier clase que implemente esta interfaz debe proporcionar una implementación de esta función. 
};
