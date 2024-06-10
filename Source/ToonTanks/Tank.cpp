#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Blueprint/UserWidget.h"
ATank::ATank()
{
    // Set this pawn to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

// Called to bind functionality to input
void ATank::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Turn);
    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
    PlayerInputComponent->BindAction(TEXT("RightMouseButton"), IE_Pressed, this, &ATank::ResumeGame);
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PlayerControllerRef = Cast<APlayerController>(GetController());
    if (PlayerControllerRef)
    {
        FHitResult HitResult;
        PlayerControllerRef->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);
        // DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 25.f, 12, FColor::Red, false, -1.f);
        RotateTurret(HitResult.ImpactPoint);
    }
    DetectAnomaly();
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
    Super::BeginPlay();

    // Start logging movement data
    GetWorldTimerManager().SetTimer(MovementLogTimerHandle, this, &ATank::LogMovementData, MovementLogInterval, true);
}

void ATank::Move(float Value)
{
    FVector DeltaLocation = FVector::ZeroVector;
    DeltaLocation.X = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * Speed;
    AddActorLocalOffset(DeltaLocation, true);
}

void ATank::Turn(float Value)
{
    FRotator DeltaRotation = FRotator::ZeroRotator;
    // Yaw = Value * DeltaTime * TurnRate
    DeltaRotation.Yaw = Value * TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
    AddActorLocalRotation(DeltaRotation, true);
}

void ATank::LogMovementData()
{
    FVector Position = GetActorLocation();
    FRotator Rotation = GetActorRotation();
    float Timestamp = GetWorld()->GetTimeSeconds();

    FString LogEntry = FString::Printf(TEXT("%f, %f, %f, %f, %f, %f, %f\n"),
                                       Timestamp, Position.X, Position.Y, Position.Z,
                                       Rotation.Pitch, Rotation.Yaw, Rotation.Roll);

    FString LogFilePath = FPaths::ProjectDir() + TEXT("MovementLog.csv");

    // Check if the file exists and create a new one if it doesn't
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*LogFilePath))
    {
        FFileHelper::SaveStringToFile(TEXT("Timestamp,PosX,PosY,PosZ,Pitch,Yaw,Roll\n"), *LogFilePath);
    }

    // Append the log entry to the file
    FFileHelper::SaveStringToFile(LogEntry, *LogFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}

void ATank::DetectAnomaly()
{
    FString LogFilePath = FPaths::ProjectDir() + TEXT("MovementLog.csv");

    TArray<FString> LogLines;
    if (FFileHelper::LoadFileToStringArray(LogLines, *LogFilePath))
    {
        float prevYawValue = GetActorRotation().Yaw;
        int32 anomalyCount = 0;
        int32 consecCount = 0; // Move declaration outside the loop
        // Loop through each line in the log file
        for (const FString &LogLine : LogLines)
        {
            // Split the line into individual values
            TArray<FString> LogValues;
            LogLine.ParseIntoArray(LogValues, TEXT(","), true);

            // Ensure the line has enough values

            // Extract the yaw value
            float YawValue = FCString::Atof(*LogValues[5]); // Assuming yaw is the 6th value (index 5)

            // Do something with the yaw value
            if (YawValue == prevYawValue)
            {
                consecCount++;
                if (consecCount >= 3)
                {
                    anomalyCount++;
                    if (anomalyCount == 3)
                    {
                        Speed = 0;
                        TurnRate = 0;
                        ShowPopup();
                        return; // Exit the function when anomalyCount is 5
                    }
                    consecCount = 0;
                }
            }
            else
            {
                consecCount = 0; // Reset consecCount if YawValue changes
            }
            prevYawValue = YawValue;
            // For example, print it to the screen
            UE_LOG(LogTemp, Warning, TEXT("Yaw Value: %f"), YawValue);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to read log file: %s"), *LogFilePath);
    }
}
void ATank::ResumeGame()
{
    Speed = 700.f;
    TurnRate = 120.f;
    ClearMovementLog();
}

void ATank::ClearMovementLog()
{
    FString LogFilePath = FPaths::ProjectDir() + TEXT("MovementLog.csv");

    // Clear the contents of the file
    FFileHelper::SaveStringToFile(TEXT(""), *LogFilePath);
}
void ATank::ShowPopup()
{
}
