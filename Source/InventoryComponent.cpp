// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::AddItem(FItemDefinition NewItemData)
{
	// New Object asignael UObject en el pool de memoria gestinoada por Unreal
	UInventoryItemInstance* NewItem = NewObject<UInventoryItemInstance>(this);
	NewItem->Data = NewItemData;
	// Al añadirlo el TArray marcado con UPROPERTY(), el GC sabe que tiene una referencia activa
	Items.Add(NewItem);

	// Asignamos el puntero débil, si Items.Remove() se ejecuta y el GC limpia, este puntero se volverá nulo automáticamente
	InspectedItem = NewItem;
	
	UE_LOG(LogTemp, Log, TEXT("Item %s added:"), *NewItemData.ItemName.ToString());
}

void UInventoryComponent::EquipItemAsync(int32 ItemIndex, UStaticMeshComponent * TargetMeshComponent)
{
	if (!Items.IsValidIndex(ItemIndex) || !TargetMeshComponent)
	{
		return;
	}

	// PASO CLAVE: Si había otra carga en proceso (el jugador cambió rápido de ítem), la cancelamos
	CancelCurrentLoad();

	//Si ya está en memoria, lo asignamos directamente
	TSoftObjectPtr<UStaticMesh> MeshSoftPtr = Items[ItemIndex]->Data.VisualMesh;

	if (MeshSoftPtr.IsNull())
	{
		TargetMeshComponent->SetStaticMesh(nullptr);
		return;
	}

	// Caso A: Ya está en memoria RAM/VRAM
	if (MeshSoftPtr.IsValid())
	{
		TargetMeshComponent->SetStaticMesh(MeshSoftPtr.Get());
		// Informamos al HUD que ya está completado
		OnLoadingProgressUpdated.Broadcast(1.0f);
		return;
	}

	// Caso B: Carga asíncrona requerida
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange,
			FString::Printf(TEXT("[Streaming] Solicitando carga de: %s"), *MeshSoftPtr.GetAssetName()));
	}

	// Si no está cargado, realizamos la carga asíncrona sin congelar el juego
	AssetLoadHandle = StreamableManager.RequestAsyncLoad(MeshSoftPtr.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UInventoryComponent::OnMeshLoaded, MeshSoftPtr, TargetMeshComponent));

	// Activamos un timer a 60 Hz (~0.016s) para actualizar la barra fluidamente
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ProgressTimerHandle,
			this,
			&UInventoryComponent::CheckLoadingProgress,
			0.016f,
			true
		);
	}
}

void UInventoryComponent::CancelCurrentLoad()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	if (AssetLoadHandle.IsValid() && AssetLoadHandle->IsLoadingInProgress())
	{
		// 1. Cancela el stream pendiente y anula la ejecución del delegate encolado
		AssetLoadHandle->CancelHandle();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow,
				TEXT("[Streaming] Carga asíncrona anterior cancelada con éxito."));
		}
	}

	// 2. Liberamos la referencia del puntero compartido
	AssetLoadHandle.Reset();
	OnLoadingProgressUpdated.Broadcast(0.0f);
}

void UInventoryComponent::RunGCTest()
{
	UE_LOG(LogTemp, Log, TEXT("Running GC Test..."));

	if (InspectedItem.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Antes e vaciarÑ InspectedItem es válido (%s)"), *InspectedItem->Data.ItemName.ToString());
	}

	// Rompemos la [unica referencias fuerte que el gc conoce
	Items.Empty();

	// Forzamos al motor a ejecutar una pasada completa de Garbage Collection inmediatamente
	GEngine->ForceGarbageCollection(true);

	// Verificamos el estado del TWeakObjectPtr
	if (!InspectedItem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Después de GC: InspectedItemd se invalidó de forma segura sin causar crash."))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("El objecto a[un est[a en memoria."));
	}
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CancelCurrentLoad();
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::CheckLoadingProgress()
{
	if (AssetLoadHandle.IsValid() && AssetLoadHandle->IsLoadingInProgress())
	{
		// GetProgress() devuelve un float de 0.0f a 1.0f
		const float Progress = AssetLoadHandle->GetProgress();

		// 1. Emitir evento hacia Blueprints / UMG
		OnLoadingProgressUpdated.Broadcast(Progress);

		// 2. Feedback visual en el Viewport con la tecla de debug única (ID: 101) para sobreescribir la misma línea
		if (GEngine)
		{
			FString ProgressStr = FString::Printf(TEXT("[Streaming] Progreso: %.1f%%"), Progress * 100.0f);
			GEngine->AddOnScreenDebugMessage(101, 0.1f, FColor::Yellow, ProgressStr);
		}
	}
}

void UInventoryComponent::OnMeshLoaded(TSoftObjectPtr<UStaticMesh> MeshPath, UStaticMeshComponent* TargetMeshComponent)
{
	// Detener el sondeo del temporizador
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ProgressTimerHandle);
	}

	// Si el handle fue cancelado o reiniciado mientras este callback entraba al Game Thread, descartamos la acción
	if (!AssetLoadHandle.IsValid())
	{
		return;
	}

	if (TargetMeshComponent && MeshPath.IsValid())
	{
		TargetMeshComponent->SetStaticMesh(MeshPath.Get());

		// Notificar el 100% final al HUD
		OnLoadingProgressUpdated.Broadcast(1.0f);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan,
			FString::Printf(TEXT("[Streaming] Mesh aplicado: %s"), *MeshPath.GetAssetName()));
	}
}

