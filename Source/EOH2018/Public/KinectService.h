#pragma once

#include "EOH2018.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KinectService.generated.h"

USTRUCT()
struct FKinectJoint {
  GENERATED_BODY()
  UPROPERTY() float x;
  UPROPERTY() float y;
  UPROPERTY() float z;
};

USTRUCT()
struct FKinectSkeleton {
  GENERATED_BODY()
  UPROPERTY() FKinectJoint head;
  UPROPERTY() FKinectJoint neck;
  UPROPERTY() FKinectJoint left_shoulder;
  UPROPERTY() FKinectJoint right_shoulder;
  UPROPERTY() FKinectJoint left_elbow;
  UPROPERTY() FKinectJoint right_elbow;
  UPROPERTY() FKinectJoint left_hand;
  UPROPERTY() FKinectJoint right_hand;
  UPROPERTY() FKinectJoint torso;
  UPROPERTY() FKinectJoint left_hip;
  UPROPERTY() FKinectJoint right_hip;
  UPROPERTY() FKinectJoint left_knee;
  UPROPERTY() FKinectJoint right_knee;
  UPROPERTY() FKinectJoint left_foot;
  UPROPERTY() FKinectJoint right_foot;
};

USTRUCT()
struct FKinectWrapper {
  GENERATED_BODY()
  UPROPERTY() TArray<FKinectSkeleton> data;
};

UCLASS()
class EOH2018_API AKinectService : public AActor
{
	GENERATED_BODY()

  public:
	AKinectService();

 protected:
	virtual void BeginPlay() override;

 public:
	virtual void Tick(float DeltaTime) override;
  UFUNCTION(BlueprintCallable)
  int GetUserNumber();
  UFUNCTION(BlueprintCallable)
  FVector GetLeftHand(int userIndex);
  UFUNCTION(BlueprintCallable)
  FVector GetRightHand(int userIndex);

  UFUNCTION(BlueprintCallable)
  FVector RealToScreen(const FVector& real);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float ScreenScale = 0.1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float ScreenDepth = 200;

  UFUNCTION(BlueprintCallable)
  FVector GetPrimaryHandScreenPosition(int userIndex);

 private:
  FKinectWrapper skeletonData;
  bool updating = false;
  FHttpModule* Http;
  FString DataUrl = "http://localhost:3000/data/";
  void SendRequest();
  bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);
  void DataResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
