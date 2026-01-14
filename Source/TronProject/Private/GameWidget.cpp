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
	
}

void UGameWidget::Countdown(int32 CountdownIndex){
	if (CountdownIndex > 4) {
		CountdownTextBlock->SetText(FText::GetEmpty());
		return;
	}
	CountdownTextBlock->SetText(CountdownText[CountdownIndex]);
}
