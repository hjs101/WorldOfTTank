#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckForNearbyObstacle.generated.h"

/**
 * 
 */
UCLASS()
class WORLDOFTTANK_API UBTTask_CheckForNearbyObstacle : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_CheckForNearbyObstacle();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
