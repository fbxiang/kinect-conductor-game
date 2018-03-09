// Fill out your copyright notice in the Description page of Project Settings.
#include "Metronome.h"

// Sets default values
AMetronome::AMetronome()
{
	PrimaryActorTick.bCanEverTick = false; // we manage our own ticks
  InstrumentStates.SetNum(MAX_INSTRUMENTS); // we use 4 instruments
}

// Called when the game starts or when spawned
void AMetronome::BeginPlay()
{
	Super::BeginPlay();
  CurrentMaxTime = 8;
  GlobalTimeStep = 0;
  Http = &FHttpModule::Get();
  if (!UMidiInterface::isOutputOpen() && !UMidiInterface::OpenMidiOutput()) {
    GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, "Cannot Open Midi Device");
  }
  NextUnit();
}

void AMetronome::SendRequest() {
  TSharedRef<IHttpRequest> Request = Http->CreateRequest();
  Request->SetURL(DataUrl);
  Request->SetVerb("GET");
  Request->OnProcessRequestComplete().BindUObject(this, &AMetronome::ProcessResponse);
  Request->ProcessRequest();
}

bool AMetronome::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
  GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red,
                                   "Error code:" + FString::FromInt(Response->GetResponseCode()));
  return false;
}

void AMetronome::ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
  if (ResponseIsValid(Response, bWasSuccessful)){
    FMusicPattern pattern;
    FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &pattern, 0, 0);
    for (int i = 0; i < pattern.Notes.Num(); i++) {
      if (pattern.Notes[i].Time < 0 || pattern.Notes[i].Time >= pattern.Length)
        continue; // this is to avoid silly input by our teammate
      if (pattern.Notes[i].Instrument < 0 || pattern.Notes[i].Instrument >= 4)
        continue;
      CurrentNotes.Add(FNoteInfo(pattern.Notes[i].Time + CurrentMaxTime, pattern.Notes[i].Instrument,
                                 pattern.Notes[i].Notes));
    }
    CurrentMaxTime += pattern.Length;
  }
  Updating = false;
  // otherwise we are doomed
}

// Called every frame
void AMetronome::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMetronome::NextUnit() {
  if (!Updating && (GlobalTimeStep == 0 || GlobalTimeStep + 15 >= CurrentMaxTime)) {
    Updating = true;
    SendRequest();
    if (GlobalTimeStep == 0) {
      GlobalTimeStep = 1;
    }
  }

  // release the instruments when necessary
  for (int i = 0; i < InstrumentStates.Num(); i++) {
    FInstrumentState& state = InstrumentStates[i];
    TArray<int> keys;
    state.NoteReleaseTime.GetKeys(keys);
    for (int& key : keys) {
      int* time = state.NoteReleaseTime.Find(key);
      if (*time >= GlobalTimeStep) {
        ReleaseNote(key, i);
        state.NoteReleaseTime.Remove(key);
      }
    }
  }

  if (CurrentMaxTime > 8) { // received some packet, game started
    GlobalTimeTick(GlobalTimeStep);
    ++GlobalTimeStep;
  }
  GetWorld()->GetTimerManager().SetTimer(UnitTimerHandle, this, &AMetronome::NextUnit, UnitLength, false);
}

void AMetronome::PressNote(int note, int instrument) {
  if (!UMidiInterface::isOutputOpen()) {
    return;
  }
  FMidiEvent event;
  event.Type = EMidiTypeEnum::MTE_NOTE;
  event.Channel = instrument + 1;
  event.Data1 = note;
  event.Data2 = 100;
  // GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Green, "Sending Midi Event Press");
  UMidiInterface::SendMidiEvent(event);
}

void AMetronome::ReleaseNote(int note, int instrument) {
  if (!UMidiInterface::isOutputOpen()) {
    return;
  }
  FMidiEvent event;
  event.Type = EMidiTypeEnum::MTE_NOTE_OFF;
  event.Channel = instrument;
  event.Data1 = note;
  event.Data2 = 10;
  UMidiInterface::SendMidiEvent(event);
}

void AMetronome::QueueForPlay(int note, int instrument, int time) {
  if (time < GlobalTimeStep) return; // play time is passed
  if (instrument < 0 || instrument >= MAX_INSTRUMENTS) return;
  int* state = InstrumentStates[instrument].NoteReleaseTime.Find(note);
  if (UMidiInterface::isOutputOpen()) {
    if (state) {
      ReleaseNote(note, instrument);
    }
    PressNote(note, instrument);
    InstrumentStates[instrument].NoteReleaseTime.Add(note, time + 6); // assume duration is 1 for now
  }
}

bool AMetronome::HasNextNote() {
  return CurrentNotes.Num() > 0;
}

FNoteInfo AMetronome::GetNextNote() {
  if (CurrentNotes.Num())
    return CurrentNotes[0];
  return FNoteInfo();
}

void AMetronome::RemoveNextNote() {
  if (CurrentNotes.Num()) {
    CurrentNotes.RemoveAt(0);
  }
}

FString AMetronome::GetDebugInformation() {
  return "Current Time: " + FString::FromInt(GlobalTimeStep) +
  "\nArray Length: " + FString::FromInt(CurrentNotes.Num()) +
  "\nFirst Element: " + (HasNextNote() ? FString::FromInt(GetNextNote().Time) : "None") + "\n";
}
