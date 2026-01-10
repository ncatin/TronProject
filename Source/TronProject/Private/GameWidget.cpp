// Fill out your copyright notice in the Description page of Project Settings.


#include "GameWidget.h"
#include "TimerManager.h"
#include "TronGameMode.h"
#include "TronPlayerController.h"
#include <Kismet/GameplayStatics.h>

void UGameWidget::NativeConstruct(){
	Super::NativeConstruct();
}

UGameWidget::UGameWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer){
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(RepeatingHandle, this, &UGameWidget::Countdown, 1, true);
}

void UGameWidget::Countdown(){
	if (CountdownIndex > 4) {
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(RepeatingHandle);
		ATronPlayerController* Owner = Cast<ATronPlayerController>(GetOwningPlayer());
		Owner->SetSpeed(700);
	}
	CountdownTextBlock->SetText(CountdownText[CountdownIndex++]);
	
}
