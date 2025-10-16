//=== General Includes ===
#include "stdafx.h"
#include "EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
using namespace Elite;

//-----------------------------------------------------------------
// BEHAVIOR TREE COMPOSITES (IBehavior)
//-----------------------------------------------------------------
#pragma region COMPOSITES
//SELECTOR
BehaviorState BehaviorSelector::Execute(Blackboard* pBlackBoard)
{
	// BT TODO:
	//TODO: Fill in this code
	// Loop over all children in m_ChildBehaviors
	for (const auto child : m_ChildBehaviors)
	{
		//Every Child: Execute and store the result in m_CurrentState
		m_CurrentState = child->Execute(pBlackBoard);

		//Check the currentstate and apply the selector Logic:
		//if a child returns Success:
		if (m_CurrentState == BehaviorState::Success)
		{
			//stop looping over all children and return Success
			return m_CurrentState;
		}
		
		//if a child returns Running:
		if (m_CurrentState == BehaviorState::Running)
		{
			//Running: stop looping and return Running
			return m_CurrentState;
		}
		//The selector fails if all children failed.
	}
	//All children failed
	m_CurrentState = BehaviorState::Failure;
	return m_CurrentState;
}
//SEQUENCE
BehaviorState BehaviorSequence::Execute(Blackboard* pBlackBoard)
{
	// BT TODO:
	//TODO: FIll in this code
	//Loop over all children in m_ChildBehaviors
	for (const auto child : m_ChildBehaviors)
	{
		//Every Child: Execute and store the result in m_CurrentState
		m_CurrentState = child->Execute(pBlackBoard);

		//Check the currentstate and apply the sequence Logic:
		//if a child returns Failed:
		if (m_CurrentState == BehaviorState::Failure)
		{
			//stop looping over all children and return Failed
			return m_CurrentState;
		}
		
		//if a child returns Running:
		if (m_CurrentState == BehaviorState::Running)
		{
			//Running: stop looping and return Running
			return m_CurrentState;
		}
		//The selector succeeds if all children succeeded.
	}
	//All children succeeded 
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;
}
//PARTIAL SEQUENCE
BehaviorState BehaviorPartialSequence::Execute(Blackboard* pBlackBoard)
{
	while (m_CurrentBehaviorIndex < m_ChildBehaviors.size())
	{
		m_CurrentState = m_ChildBehaviors[m_CurrentBehaviorIndex]->Execute(pBlackBoard);
		switch (m_CurrentState)
		{
		case BehaviorState::Failure:
			m_CurrentBehaviorIndex = 0;
			return m_CurrentState;
		case BehaviorState::Success:
			++m_CurrentBehaviorIndex;
			m_CurrentState = BehaviorState::Running;
			return m_CurrentState;
		case BehaviorState::Running:
			return m_CurrentState;
		}
	}

	m_CurrentBehaviorIndex = 0;
	m_CurrentState = BehaviorState::Success;
	return m_CurrentState;
}
#pragma endregion
//-----------------------------------------------------------------
// BEHAVIOR TREE CONDITIONAL (IBehavior)
//-----------------------------------------------------------------
BehaviorState BehaviorConditional::Execute(Blackboard* pBlackBoard)
{
	if (m_fpConditional == nullptr)
		return BehaviorState::Failure;

	// This used to be a switch case for some reason, now it's not, be happy :)
	if (m_fpConditional(pBlackBoard))
	{
		m_CurrentState = BehaviorState::Success;
	}
	else
	{
		m_CurrentState = BehaviorState::Failure;
	}
	return m_CurrentState;
}

BehaviorState BehaviorConditionalInverter::Execute(Blackboard* pBlackBoard)
{
	const auto state = BehaviorConditional::Execute(pBlackBoard);
	return state == BehaviorState::Success ? BehaviorState::Failure : BehaviorState::Success;
}
//-----------------------------------------------------------------
// BEHAVIOR TREE ACTION (IBehavior)
//-----------------------------------------------------------------
BehaviorState BehaviorAction::Execute(Blackboard* pBlackBoard)
{
	if (m_fpAction == nullptr)
		return BehaviorState::Failure;

	m_CurrentState = m_fpAction(pBlackBoard);
	return m_CurrentState;
}

BehaviorState BehaviorActionInverter::Execute(Blackboard* pBlackBoard)
{
	const auto state = BehaviorAction::Execute(pBlackBoard);
	return state == BehaviorState::Success ? BehaviorState::Failure : BehaviorState::Success;
}
