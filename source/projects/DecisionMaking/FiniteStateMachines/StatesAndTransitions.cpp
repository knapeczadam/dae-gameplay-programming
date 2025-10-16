#include "stdafx.h"
#include "StatesAndTransitions.h"

#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"
#include "projects/Shared/NavigationColliderElement.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

// Available blackboard keys:
// --Agents--
// - "Player"            : SteeringAgent*
// - "Guard"             : SmartAgent*
// - "PlayerPathFollow"  : PathFollow*
// - "GuardPathFollow"   : PathFollow*
// - "Target"            : TargetData
// - "AgentRadius"       : float
// - "PlayerSpeed"       : float
// - "GuardSpeed"        : float
// - "DetectionRadius"   : float
// - "MaxSearchTime"     : float
// - "AccuSearchTime"    : float
// - "GuardViewAngle"    : float
// - "GuardViewDistance" : float
// - "DebugHelper"       : DebugHelper

// --Level--
// - "NavigationColliders" : std::vector<NavigationColliderElement*>

// --Pathfinder--
// - "PlayerPath" : std::vector<Elite::Vector2>
// - "PatrolPath" : std::vector<Elite::Vector2>

// --Graph--
// - "NavGraph" : Elite::NavGraph*

//------------
//---STATES---
//------------
// PatrolState
void PatrolState::OnEnter(Blackboard* pBlackboard)
{
    // Set behavior to PathFollow
    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);

    PathFollow* pPathFollow = nullptr;
    pBlackboard->GetData("GuardPathFollow", pPathFollow);

    pGuard->SetSteeringBehavior(pPathFollow);
}

void PatrolState::Update(Blackboard* pBlackboard, float deltaTime)
{
    PathFollow* pPathFollow = nullptr;
    pBlackboard->GetData("GuardPathFollow", pPathFollow);

    std::vector<Elite::Vector2> patrolPath;
    pBlackboard->GetData("PatrolPath", patrolPath);

    if (pPathFollow->HasArrived())
    {
        pPathFollow->SetPath(patrolPath);
    }
}

void PatrolState::OnExit(Blackboard* pBlackboard)
{
    // Set player's last known position as target
    TargetData* pTarget = nullptr;
    pBlackboard->GetData("Target", pTarget);

    SteeringAgent* pPlayer = nullptr;
    pBlackboard->GetData("Player", pPlayer);

    pTarget->Position = pPlayer->GetPosition();
    pBlackboard->ChangeData("Target", pTarget);
}

// ChaseState
void ChaseState::OnEnter(Blackboard* pBlackboard)
{
    // Set behavior to Seek
    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);

    Seek* pSeek = pGuard->GetSeekBehavior();
    pGuard->SetSteeringBehavior(pSeek);
}

void ChaseState::Update(Blackboard* pBlackboard, float deltaTime)
{
    TargetData target;

    SteeringAgent* pPlayer = nullptr;
    pBlackboard->GetData("Player", pPlayer);

    target.Position = pPlayer->GetPosition();

    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);

    Seek* pSeek = pGuard->GetSeekBehavior();
    pSeek->SetTarget(target);
}

void ChaseState::OnExit(Blackboard* pBlackboard)
{
    // Set player's last known position as target
    TargetData* pTarget = nullptr;
    pBlackboard->GetData("Target", pTarget);

    SteeringAgent* pPlayer = nullptr;
    pBlackboard->GetData("Player", pPlayer);

    pTarget->Position = pPlayer->GetPosition();
    pBlackboard->ChangeData("Target", pTarget);
}

// SearchState
void SearchState::OnEnter(Blackboard* pBlackboard)
{
    // Set behavior to Seek
    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);

    Seek* pSeek = pGuard->GetSeekBehavior();
    pGuard->SetSteeringBehavior(pSeek);
}

void SearchState::Update(Blackboard* pBlackboard, float deltaTime)
{
    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);

    TargetData* pTarget = nullptr;
    pBlackboard->GetData("Target", pTarget);

    Wander* pWander = pGuard->GetWanderBehavior();
    Seek* pSeek = pGuard->GetSeekBehavior();
    pSeek->SetTarget(*pTarget);

    if (pGuard->GetSteeringBehavior() == pSeek)
    {
        if (pGuard->GetPosition().DistanceSquared(pTarget->Position) < 1.0f)
        {
            pGuard->SetSteeringBehavior(pWander);
        }
    }

    if (pWander == pGuard->GetSteeringBehavior())
    {
        float accuSearchTime = 0.f;
        pBlackboard->GetData("AccuSearchTime", accuSearchTime);
        accuSearchTime += deltaTime;
        pBlackboard->ChangeData("AccuSearchTime", accuSearchTime);
    }
}

void SearchState::OnExit(Blackboard* pBlackboard)
{
    // Set player's last known position as target
    TargetData* pTarget = nullptr;
    pBlackboard->GetData("Target", pTarget);

    SteeringAgent* pPlayer = nullptr;
    pBlackboard->GetData("Player", pPlayer);

    pTarget->Position = pPlayer->GetPosition();
    pBlackboard->ChangeData("Target", pTarget);
}

//-----------------
//---TRANSITIONS---
//-----------------
/**
 * \brief Used to check if the player is within the detection radius and line of sight of the guard
 * \param pBlackboard 
 * \return 
 */
bool IsTargetVisible::Evaluate(Blackboard* pBlackboard) const
{
    SteeringAgent* pPlayer = nullptr;
    pBlackboard->GetData("Player", pPlayer);

    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);

    float detectionRadius = 0.0f;
    pBlackboard->GetData("DetectionRadius", detectionRadius);
    
    DebugHelper* pDebugHelper = nullptr;
    pBlackboard->GetData("DebugHelper", pDebugHelper);

    const Elite::Vector2 playerPos = pPlayer->GetPosition();
    const Elite::Vector2 guardPos = pGuard->GetPosition();
    const float distanceSqToPlayer = Elite::DistanceSquared(playerPos, guardPos);


    if (pDebugHelper->m_EnableGuardViewCone)
    {
        if (Elite::IsPointInTriangle(playerPos, guardPos, pDebugHelper->m_GuardViewConeLeft, pDebugHelper->m_GuardViewConeRight))
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

bool IsTargetNotVisible::Evaluate(Blackboard* pBlackboard) const
{
    SteeringAgent* pPlayer = nullptr;
    pBlackboard->GetData("Player", pPlayer);
    SmartAgent* pGuard = nullptr;
    pBlackboard->GetData("Guard", pGuard);
    float detectionRadius = 0.f;
    pBlackboard->GetData("DetectionRadius", detectionRadius);

    const Elite::Vector2 playerPos = pPlayer->GetPosition();
    const Elite::Vector2 guardPos = pGuard->GetPosition();
    Elite::Vector2 toPlayer = playerPos - guardPos;
    const float distanceToPlayer = Elite::DistanceSquared(playerPos, guardPos);
    if (distanceToPlayer > detectionRadius * detectionRadius)
    {
        return true;
    }
    return false;
}

/**
 * \brief Used to check if the guard, who’s been searching has been doing so for too long,
 * \param pBlackboard 
 * \return 
 */
bool IsSearchingTooLong::Evaluate(Blackboard* pBlackboard) const
{
    float maxSearchTime = 0.f;
    pBlackboard->GetData("MaxSearchTime", maxSearchTime);
    float accuSearchTime = 0.f;
    pBlackboard->GetData("AccuSearchTime", accuSearchTime);
    if (accuSearchTime > maxSearchTime)
    {
        pBlackboard->ChangeData("AccuSearchTime", 0.0f);
        std::cout << "Search time exceeded!\n";
        return true;
    }
    return false;
}
