#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent; //미리선언 이런게 있다~ 라고 미리 알려주는거임
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float Health;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount, // 데미지 양, 얼마나 데미지를 입었나
		struct FDamageEvent const& DamageEvent, // 데미지를 받은 유형 -> 나중에 확장해서 구현할때 불속성 물속성 등등 쓸수있음
		AController* EventInstigator, // 데미지를 누가 입혔는지 (지뢰를 심은 사람?) 지금은 쓸 일없음  null값이 될듯
		AActor* DamageCauser) override; // 데미지를 입힌 오브젝트 (지뢰)

	UFUNCTION()
	void Move(const FInputActionValue& value);  //얘내들 다 리플렉션 시스템에 등록되어있어야함
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void OnDeath();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};
