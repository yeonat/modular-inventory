// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlayerController.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

void ATestPlayerController::AddItem(int32 ItemID)
{
	UInventoryComponent* Inventory = GetInventory();
	if (!Inventory)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[Error] No se encontró InventoryComponent en el Pawn actual."));
		return;
	}

	FItemDefinition NewItem = CreateDummyItem(ItemID);
	Inventory->AddItem(NewItem);
}

void ATestPlayerController::EquipItem(int32 ItemIndex)
{
	UInventoryComponent* Inventory = GetInventory();
	UStaticMeshComponent* MeshComponent = GetTargetMeshComponent();
	if (!Inventory || !MeshComponent)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[Error] Falta InventoryComponent o StaticMeshComponent en el Pawn."));
		return;
	}

	return Inventory->EquipItemAsync(ItemIndex, MeshComponent);
}

void ATestPlayerController::TestGC()
{
	UInventoryComponent* Inventory = GetInventory();
	if (!Inventory)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[Error] No se encontró InventoryComponent en el Pawn."));
		return;
	}
	Inventory->RunGCTest();
}

UInventoryComponent* ATestPlayerController::GetInventory() const
{
	APawn* CurrentPawn = GetPawn();
	if (!CurrentPawn)
	{
		return nullptr;
	}

	return CurrentPawn->FindComponentByClass<UInventoryComponent>();
}

UStaticMeshComponent* ATestPlayerController::GetTargetMeshComponent() const
{
	APawn* CurrentPawn = GetPawn();
	if (!CurrentPawn)
	{
		return nullptr;
	}

	return CurrentPawn->FindComponentByClass<UStaticMeshComponent>();
}

FItemDefinition ATestPlayerController::CreateDummyItem(int32 ItemID)
{
	FItemDefinition Item;

	switch (ItemID)
	{
		case 1:
			Item.ItemName = FText::FromString(TEXT("Cubo Básico"));
			Item.PowerValue = 15.f;
			// Asset estándar del motor para pruebas sin assets propios
			Item.VisualMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Engine/BasicShapes/Cube.Cube")));
			break;
		case 2:
			Item.ItemName = FText::FromString(TEXT("Esfera Básica"));
			Item.PowerValue = 50.f;
			// Asset estándar del motor para pruebas sin assets propios
			Item.VisualMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Engine/BasicShapes/Sphere.Sphere")));
			break;
		case 3:
			Item.ItemName = FText::FromString(TEXT("Cilindro Básico"));
			Item.PowerValue = 85.f;
			// Asset estándar del motor para pruebas sin assets propios
			Item.VisualMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Engine/BasicShapes/Cone.Cone")));
			break;
		default:
			Item.ItemName = FText::FromString(FString::Printf(TEXT("Item Genérico #%d"), ItemID));
			Item.PowerValue = 5.0f;
			Item.VisualMesh = nullptr;
			break;
	}

	return Item;
}
