
#include "SpartaCharacter.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprongArm")); //컴포넌트 추가
	SpringArmComp->SetupAttachment(RootComponent); //스프링 암을 어디다 어태치 할 것인가? Root component에 부착
	SpringArmComp->TargetArmLength = 300.0f; // 스프링 암의 길이 설정
	SpringArmComp->bUsePawnControlRotation = true; //시점 회전할때 스프링암도 같이 움직이게 하기위한 설정

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); //카메라를 어디다 부착할 것인가? Spring Arm에 부착 하지만 Spring Arm 어디에? SocketName(스프링 암 끝부분)에
	CameraComp->bUsePawnControlRotation = false; //시점 회전할때 카메라는 스프링암에 붙어있어야함 그래서 얜 끔

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;

}




void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction) //널이 아닌지 체크
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}

			if (PlayerController->JumpAction) 
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction) 
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& value) //여기서 value는 2D Vector였음
{
	if (!Controller) return; //컨트롤러가 유호한지 확인 안정화 코드

	const FVector2D MoveInput = value.Get<FVector2D>(); //변수선언 밸류에서 2D 구조체 형태로 가져와서 무브인풋에다가 2D벡터 형태로 변환해서 저장

	if (!FMath::IsNearlyZero(MoveInput.X)) //0이랑 가까우면 0으로 하자 !! 이런느낌 앞에 !가 붙었으니 0이 아닐때 실행하는 느낌스
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X); //캐릭터 클래스는 무브먼트 컴포턴트가 이미 내부적으로 구현되있음 그래서 그냥 가져온 것 여기서 인자는 캐릭터의 정면방향 X만큼 이동시켜주세요
	}

	if (!FMath::IsNearlyZero(MoveInput.Y)) //0이랑 가까우면 0으로 하자 !! 이런느낌 앞에 !가 붙었으니 0이 아닐때 실행하는 느낌스
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}
void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump(); // 기본제공함수
	}
}
void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping(); // 기본제공함수
	}
}
void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X); // 좌 우
	AddControllerPitchInput(LookInput.Y); // 위 아래 IA만들때 이미 반전해줬기 때문에 이대로 쓰면댐
}
void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}
void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health increased to: %f"), Health); //체력이 이 값으로 회복되었다 !

}

float ASpartaCharacter::TakeDamage(
	float DamageAmount, // 데미지 양, 얼마나 데미지를 입었나
	struct FDamageEvent const& DamageEvent, // 데미지를 받은 유형 -> 나중에 확장해서 구현할때 불속성 물속성 등등 쓸수있음
	AController* EventInstigator, // 데미지를 누가 입혔는지 (지뢰를 심은 사람?) 지금은 쓸 일없음  null값이 될듯
	AActor* DamageCauser) // 데미지를 입힌 오브젝트 (지뢰)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); //ActualDamage 실제로 들어온 데미지 ! (방어구같은거 있으면 데미지가 경감되기때문)

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health); //체력이 이 값으로 감소되었다 !

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	//게임 종료 로직
}