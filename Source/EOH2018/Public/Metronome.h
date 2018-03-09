// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EOH2018.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MidiInterfaceLibrary.h"
#include "Metronome.generated.h"

#define MAX_INSTRUMENTS 4

USTRUCT()
struct FInstrumentState {
  GENERATED_BODY()
  UPROPERTY() TMap<int, int> NoteReleaseTime;
};

USTRUCT(BlueprintType)
struct FNoteInfo {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Note Info Struct")
  int Time;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Note Info Struct")
  int Instrument;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Note Info Struct")
  TArray<int> Notes;

  FNoteInfo() {
    Time = 0; Instrument = 0; Notes = TArray<int>();
  }

  FNoteInfo(int time, int ins, TArray<int>& notes) {
    Time = time;
    Instrument = ins;
    Notes = notes;
  }
};

USTRUCT()
struct FMusicPattern {
  GENERATED_BODY()
  UPROPERTY() TArray<FNoteInfo> Notes;
  UPROPERTY() int Length;
};


UCLASS()
class EOH2018_API AMetronome : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMetronome();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
  FTimerHandle UnitTimerHandle;
  FHttpModule* Http;
  FString DataUrl = "http://localhost:5000/next";
  void SendRequest();
  bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);
  void ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
  bool Updating = false;
  TArray<FInstrumentState> InstrumentStates;
  void PressNote(int note, int instrument);
  void ReleaseNote(int note, int instrument);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  TArray<FNoteInfo> CurrentNotes;

  UPROPERTY()
  int CurrentMaxTime = 8; // for now, set 8 as the initial delay

  UPROPERTY()
  int GlobalTimeStep;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float UnitLength = 0.2;

  UFUNCTION()
  void NextUnit();

  // spawn new objects and delete old objects at the ticks
  UFUNCTION(BlueprintImplementableEvent)
  void GlobalTimeTick(int time);

  // queue notes when hit by user
  UFUNCTION(BlueprintCallable)
  void QueueForPlay(int note, int instrument, int time);

  UFUNCTION(BlueprintCallable)
  bool HasNextNote();

  // help to find notes to spawn
  UFUNCTION(BlueprintCallable)
  FNoteInfo GetNextNote();

  UFUNCTION(BlueprintCallable)
  void RemoveNextNote();

  UFUNCTION(BlueprintCallable)
  FString GetDebugInformation();
};
