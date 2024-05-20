// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void ATurret::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Find the distance to the tank
    if (InFireRange())
    {
        RotateTurret(Tank->GetActorLocation());
    }
}
void ATurret::BeginPlay()
{
    Super::BeginPlay();
    Tank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATurret::checkFireCondition, fireRate, true);
}

void ATurret::checkFireCondition()
{
    if (InFireRange())
    {
        Fire();
    }
}

bool ATurret::InFireRange()
{
    if (Tank)
    {
        float Distance = FVector::Dist(GetActorLocation(), Tank->GetActorLocation());
        if (Distance <= FireRange)
        {
            return true;
        }
    }
    return false;
}