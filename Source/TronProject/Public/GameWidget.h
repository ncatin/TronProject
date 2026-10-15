// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "GameWidget.generated.h"

/**
 * 
 */
UCLASS()
class TRONPROJECT_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	TArray<FText> CountdownText = {FText::FromString(" "), FText::FromString("3"), FText::FromString("2"), FText::FromString("1"), FText::FromString("GO!")};

public:
	UGameWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CountdownTextBlock;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* P1HorizontalBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* P2HorizontalBox;

	UFUNCTION()
	void Countdown(int32 CountdownIndex);

	FTimerHandle RepeatingHandle;
	
};
