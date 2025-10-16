#include "stdafx.h"
#include "SmartAgent.h"

SmartAgent::SmartAgent(float radius)
	: SteeringAgent(radius)
{
	m_pWander = new Wander();
	m_pSeek = new Seek();
}

SmartAgent::~SmartAgent()
{
	SAFE_DELETE(m_pDecisionMaking);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pSeek);
}

void SmartAgent::Update(float dt)
{
	if (m_pDecisionMaking)
		m_pDecisionMaking->Update(dt);

	SteeringAgent::Update(dt);
}

void SmartAgent::SetDecisionMaking(Elite::IDecisionMaking* decisionMakingStructure)
{
	SAFE_DELETE(m_pDecisionMaking); // Delete if needed
	m_pDecisionMaking = decisionMakingStructure;
}

Wander* SmartAgent::GetWanderBehavior() const
{
	return m_pWander;
}

Seek* SmartAgent::GetSeekBehavior() const
{
	return m_pSeek;
}

bool SmartAgent::HasLineOfSight(const Elite::Vector2& pos) const
{
	ClosestRaycastCallback closestCallback = ClosestRaycastCallback();
	PHYSICSWORLD->Raycast(&closestCallback, GetPosition(), pos);

	return closestCallback.GetClosestPoint().vHitShapePoints.empty();
}
