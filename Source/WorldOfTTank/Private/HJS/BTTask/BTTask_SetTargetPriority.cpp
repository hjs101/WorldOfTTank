// Fill out your copyright notice in the Description page of Project Settings.

#include "HJS/BTTask/BTTask_SetTargetPriority.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "HJS/AITankCPU_1.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_SetTargetPriority::UBTTask_SetTargetPriority()
{
	NodeName = TEXT("Set Target Priority");
}

EBTNodeResult::Type UBTTask_SetTargetPriority::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 블랙보드 불러오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	// null인지 체크
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	// 필요한 오브젝트 불러오기
	UObject* TargetPlayer = BlackboardComp->GetValueAsObject(FName("TargetPlayer"));
	UObject* TargetCPU = BlackboardComp->GetValueAsObject(FName("TargetCPU"));

	// 조건 걸기
	if (TargetPlayer && TargetCPU) 
	{
		//1. 둘 모두 다 감지된 경우

		// 30% 확률로 CPU, 70% 확률로 플레이어를 메인타겟 설정
		// 랜덤 실수 생성
		float RandomValue = UKismetMathLibrary::RandomFloatInRange(0.f, 1.f);
		// 70%
		if (RandomValue <= 0.7f) 
		{
			BlackboardComp->SetValueAsObject(FName("MainTarget"), TargetPlayer);
		}
		// 30%
		else
		{
			BlackboardComp->SetValueAsObject(FName("MainTarget"), TargetCPU);
		}
	}
	// 플레이어만 감지
	else if (TargetPlayer)
	{
		BlackboardComp->SetValueAsObject(FName("MainTarget"), TargetPlayer);
	}
	// CPU만 감지
	else if (TargetCPU)
	{
		BlackboardComp->SetValueAsObject(FName("MainTarget"), TargetCPU);
	}
	// 아무것도 감지되지 않으면 실패
	else {
		return EBTNodeResult::Failed;
	}

	// 하나라도 감지했다면 성공
	return EBTNodeResult::Succeeded;
}