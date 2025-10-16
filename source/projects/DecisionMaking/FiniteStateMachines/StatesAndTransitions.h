/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

using namespace Elite;

//------------
//---STATES---
//------------

// PatrolState
/**
 * \brief Follow the patrol path defined in App_FSM.cpp
 * Make sure to loop over the path again when the end is reached
 */
class PatrolState final : public Elite::FSMState
{
public:
    virtual void OnEnter(Blackboard* pBlackboard) override;
    virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
    virtual void OnExit(Blackboard* pBlackboard) override;
};

// ChaseState
/**
 * \brief When the player within the detection radius of the guard, and the guard has a line of sight to the
          player, make the guard follow the player
 */
class ChaseState final : public Elite::FSMState
{
public:
    virtual void OnEnter(Blackboard* pBlackboard) override;
    virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
    virtual void OnExit(Blackboard* pBlackboard) override;
};

// SearchState
/**
 * \brief When the guard doesn’t detect the player anymore (see Chase) go to the last point the player was
          spotted and then wander, searching for the player
 */
class SearchState final : public Elite::FSMState
{
public:
    virtual void OnEnter(Blackboard* pBlackboard) override;
    virtual void Update(Blackboard* pBlackboard, float deltaTime) override;
    virtual void OnExit(Blackboard* pBlackboard) override;
};
//-----------------
//---TRANSITIONS---
//-----------------
class IsTargetVisible final : public Elite::FSMCondition
{
public:
    bool Evaluate(Blackboard* pBlackboard) const override;
};

class IsTargetNotVisible final : public Elite::FSMCondition
{
public:
    bool Evaluate(Blackboard* pBlackboard) const override;
};

class IsSearchingTooLong final : public Elite::FSMCondition
{
public:
    bool Evaluate(Blackboard* pBlackboard) const override;
};

#endif