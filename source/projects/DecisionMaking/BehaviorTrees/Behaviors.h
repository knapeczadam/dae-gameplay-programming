/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
// BT TODO:

namespace BT_Actions 
{
    // Patrol
    inline Elite::BehaviorState ChangeToPatrol(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);

        PathFollow* pPathFollow = nullptr;
        pBlackboard->GetData("GuardPathFollow", pPathFollow);

        std::vector<Elite::Vector2> patrolPath;
        pBlackboard->GetData("PatrolPath", patrolPath);
#pragma endregion

        pGuard->SetSteeringBehavior(pPathFollow);
        if (pPathFollow->HasArrived())
        {
            pPathFollow->SetPath(patrolPath);
        }

        return Elite::BehaviorState::Success;
    }

    // Chase
    inline Elite::BehaviorState ChangeToChase(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
        
        SteeringAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);
#pragma endregion
        Seek* pSeek = pGuard->GetSeekBehavior();
        pGuard->SetSteeringBehavior(pSeek);
        
        pSeek->SetTarget(*pTarget);
        pGuard->SetSteeringBehavior(pSeek);

        return Elite::BehaviorState::Success;
    }

    // Search
    inline Elite::BehaviorState ChangeToSearch(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
#pragma endregion

        Wander* pWander = pGuard->GetWanderBehavior();
        pGuard->SetSteeringBehavior(pWander);

        return Elite::BehaviorState::Success;
    }

    inline Elite::BehaviorState Search(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        float deltaTime = 0.f;
        pBlackboard->GetData("DeltaTime", deltaTime);

        float accuSearchTime = 0.f;
        pBlackboard->GetData("AccuSearchTime", accuSearchTime);
#pragma endregion
        
        accuSearchTime += deltaTime;
        pBlackboard->ChangeData("AccuSearchTime", accuSearchTime);

        return Elite::BehaviorState::Success;
    }

    inline Elite::BehaviorState ResetSearchTime(Elite::Blackboard* pBlackboard)
    {
        pBlackboard->ChangeData("AccuSearchTime", 0.0f);
        
        return Elite::BehaviorState::Success;
    }

    inline Elite::BehaviorState SavePlayerPosition(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);

        SteeringAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
#pragma endregion

        pTarget->Position = pPlayer->GetPosition();
        pBlackboard->ChangeData("Target", pTarget);
        
        return Elite::BehaviorState::Success;
    }
}

namespace BT_Conditions
{
    inline bool IsPatrolling(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);

        PathFollow* pPathFollow = nullptr;
        pBlackboard->GetData("GuardPathFollow", pPathFollow);
#pragma endregion

        return pGuard->GetSteeringBehavior() == pPathFollow;
    }

    inline bool IsNotPatrolling(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);

        PathFollow* pPathFollow = nullptr;
        pBlackboard->GetData("GuardPathFollow", pPathFollow);
#pragma endregion

        return pGuard->GetSteeringBehavior() != pPathFollow;
    }

    inline bool IsChasing(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
#pragma endregion

        return pGuard->GetSteeringBehavior() == pGuard->GetSeekBehavior();
    }

    inline bool IsNotChasing(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
#pragma endregion

        return pGuard->GetSteeringBehavior() != pGuard->GetSeekBehavior();
    }

    inline bool IsSearching(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
#pragma endregion

        return pGuard->GetSteeringBehavior() == pGuard->GetWanderBehavior();
    }

    inline bool IsNotSearching(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
#pragma endregion

        return pGuard->GetSteeringBehavior() != pGuard->GetWanderBehavior();
    }
    
    inline bool IsTargetVisible(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SteeringAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);

        float detectionRadius = 0.0f;
        pBlackboard->GetData("DetectionRadius", detectionRadius);

        Elite::DebugHelper* pDebugHelper = nullptr;
        pBlackboard->GetData("DebugHelper", pDebugHelper);
#pragma endregion

        const Elite::Vector2 playerPos = pPlayer->GetPosition();
        const Elite::Vector2 guardPos = pGuard->GetPosition();
        
        const float distanceSqToPlayer = Elite::DistanceSquared(playerPos, guardPos);

        if (pDebugHelper->m_EnableGuardViewCone)
        {
            if (Elite::IsPointInTriangle(playerPos, guardPos, pDebugHelper->m_GuardViewConeLeft,
                                         pDebugHelper->m_GuardViewConeRight))
            {
                if (pDebugHelper->m_CanPlayerHide and not pDebugHelper->m_IsPlayerBehindBlock)
                {
                    std::cout << "Agent detected!\n";
                    return true;
                }
                if (not pDebugHelper->m_CanPlayerHide)
                {
                    std::cout << "Agent detected!\n";
                    return true;
                }
            }
        }
        else
        {
            if (distanceSqToPlayer < detectionRadius * detectionRadius)
            {
                if (pDebugHelper->m_CanPlayerHide and not pDebugHelper->m_IsPlayerBehindBlock)
                {
                    std::cout << "Agent detected!\n";
                    return true;
                }
                if (not pDebugHelper->m_CanPlayerHide)
                {
                    std::cout << "Agent detected!\n";
                    return true;
                }
            }
        }

        return false;
    }

    inline bool ShouldSearch(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        SteeringAgent* pPlayer = nullptr;
        pBlackboard->GetData("Player", pPlayer);
        
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
        
        float detectionRadius = 0.f;
        pBlackboard->GetData("DetectionRadius", detectionRadius);
#pragma endregion
        
        const Elite::Vector2 playerPos = pPlayer->GetPosition();
        const Elite::Vector2 guardPos = pGuard->GetPosition();
        
        const float distanceSqToPlayer = Elite::DistanceSquared(playerPos, guardPos);
        
        if (distanceSqToPlayer > detectionRadius * detectionRadius)
        {
            return true;
        }
        return false;
    }

    inline bool IsSearchingTooLong(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        float maxSearchTime = 0.f;
        pBlackboard->GetData("MaxSearchTime", maxSearchTime);
        
        float accuSearchTime = 0.f;
        pBlackboard->GetData("AccuSearchTime", accuSearchTime);
#pragma endregion
        
        if (accuSearchTime > maxSearchTime)
        {
            pBlackboard->ChangeData("AccuSearchTime", 0.0f);
            std::cout << "Search time exceeded!\n";
            return true;
        }
        return false;
    }

    inline bool IsTargetReached(Elite::Blackboard* pBlackboard)
    {
#pragma region Blackboard Data
        TargetData* pTarget = nullptr;
        pBlackboard->GetData("Target", pTarget);
        
        SmartAgent* pGuard = nullptr;
        pBlackboard->GetData("Guard", pGuard);
#pragma endregion

        return Elite::DistanceSquared(pTarget->Position, pGuard->GetPosition()) < 1.0f;
    }
}

#endif