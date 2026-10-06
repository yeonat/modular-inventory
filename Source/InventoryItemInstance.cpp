// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

void UInventoryItemInstance::ApplyModifier(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Aplicando efecto del item: %s con poder: %f"), *Data.ItemName.ToString(), Data.PowerValue);
}
