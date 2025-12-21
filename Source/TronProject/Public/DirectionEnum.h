#pragma once

#include "CoreMinimal.h"
#include "DirectionEnum.generated.h"

UENUM(BlueprintType)
enum class EMoveDirection : uint8 {
	ED_Left				UMETA(DisplayName = "Moving Left"),
	ED_Right			UMETA(DisplayName = "Moving Right"),
	ED_Up				UMETA(DisplayName = "Moving Up"),
	ED_Down				UMETA(DisplayName = "Moving Down")
};