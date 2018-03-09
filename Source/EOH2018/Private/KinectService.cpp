#include "KinectService.h"

AKinectService::AKinectService()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKinectService::BeginPlay()
{
	Super::BeginPlay();
  Http = &FHttpModule::Get();
}

void AKinectService::SendRequest() {
  updating = true;
  TSharedRef<IHttpRequest> Request = Http->CreateRequest();
  Request->SetURL(DataUrl);
  Request->SetVerb("GET");
  Request->OnProcessRequestComplete().BindUObject(this, &AKinectService::DataResponse);
  Request->ProcessRequest();
}

bool AKinectService::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	else {
    GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red,
                                     "Error code:" + FString::FromInt(Response->GetResponseCode()));
		return false;
	}
}

void AKinectService::DataResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
  if (ResponseIsValid(Response, bWasSuccessful)){
    FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &skeletonData, 0, 0);
    for (int i = 0; i < skeletonData.data.Num(); i++) {
      float x = skeletonData.data[i].left_hand.x;
      float y = skeletonData.data[i].left_hand.y;
      float z = skeletonData.data[i].left_hand.z;
    }
  }
  updating = false;
}

int AKinectService::GetUserNumber() {
  return skeletonData.data.Num();
}

FVector AKinectService::GetLeftHand(int userIndex) {
  if (userIndex < 0 || userIndex >= GetUserNumber()) {
    return FVector();
  }
  const FKinectJoint & hand = skeletonData.data[userIndex].left_hand;
  return FVector(hand.x, hand.y, hand.z);
}

FVector AKinectService::GetRightHand(int userIndex) {
  if (userIndex < 0 || userIndex >= GetUserNumber()) {
    return FVector();
  }
  const FKinectJoint & hand = skeletonData.data[userIndex].right_hand;
  return FVector(hand.x, hand.y, hand.z);
}

void AKinectService::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  if (!updating) {
    SendRequest();
  }
}

FVector AKinectService::RealToScreen(const FVector& real) {
  return FVector(ScreenDepth, -real.X * ScreenScale, real.Y * ScreenScale);
}

FVector AKinectService::GetPrimaryHandScreenPosition(int userIndex) {
  return RealToScreen(GetLeftHand(userIndex));
}
