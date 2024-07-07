#include "HJS/BTTask/BTTask_CheckForNearbyObstacle.h"
#include "HJS/AITankCPU_1.h"
#include "AIController.h"

UBTTask_CheckForNearbyObstacle::UBTTask_CheckForNearbyObstacle()
{
    NodeName = TEXT("Check For Near By Obstacle");
}

EBTNodeResult::Type UBTTask_CheckForNearbyObstacle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAITankCPU_1* AITank = Cast<AAITankCPU_1>(OwnerComp.GetAIOwner()->GetPawn());
    if (AITank && AITank->CheckForNearbyObstacle())
    {
        // 장애물이 있으면 성공
        return EBTNodeResult::Succeeded;
    }

    // 장애물이 없으면 실패
    return EBTNodeResult::Failed;
}