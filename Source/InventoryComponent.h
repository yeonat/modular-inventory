// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "InventoryItemInstance.h"
#include "InventoryComponent.generated.h"

// Delegado para enviar el porcentaje al HUD (0.0f a 1.0f)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgressUpdated, float, ProgressFraction);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MODULARINVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	// Crear un nuevo item en el inventario
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FItemDefinition NewItemData);
	// Equipar el item y cargar su modelo 3d de forma asincrona, para no bloquear el hilo principal
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItemAsync(int32 ItemIndex, UStaticMeshComponent* TargetMeshComponent);

	// Cancela cualquier carga en curso y desvincula sus callbacks
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CancelCurrentLoad();

	// Evento al que se suscribe la barra de progreso de UMG
	UPROPERTY(BlueprintAssignable, Category = "Inventory|UI")
	FOnLoadingProgressUpdated OnLoadingProgressUpdated;

	// Función de prueba de estrés para verificar el recolector de basura y la gestión de memoria
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void RunGCTest();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:

	// Punteros rastreados por el GC: NUNCA uses UInventoryItemInstance* sin UPROPERTY()
	UPROPERTY()
	TArray<UInventoryItemInstance*> Items;

	// Puntero débil a un item actualmente inspeccionado, para evitar que el GC lo recoja mientras se inspecciona
	TWeakObjectPtr<UInventoryItemInstance> InspectedItem;
	// Manejador para la carga de assets asincrona, para evitar que el GC recoja los assets mientras se cargan
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> AssetLoadHandle;

	// Manejador del timer para revisar el progreso
	FTimerHandle ProgressTimerHandle;

	void CheckLoadingProgress();
	void OnMeshLoaded(TSoftObjectPtr<UStaticMesh> MeshPath, UStaticMeshComponent* TargetMeshComponent);
};
