#include "stdafx.h"
#include "SmartAgent.h"

SmartAgent::SmartAgent(float radius)
	: SteeringAgent(radius)
{
	m_pWander           = new Wander();
	m_pSeek             = new Seek();
	m_pFace             = new Face();
	m_pRadarSeek         = new RadarSeek();
	m_pFlee             = new Flee();
	m_pEvade            = new Evade();
	m_pArrive           = new Arrive();

	m_pSteeringBehavior = m_pRadarSeek;
}

SmartAgent::~SmartAgent()
{
	SAFE_DELETE(m_pDecisionMaking);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pRadarSeek);
	SAFE_DELETE(m_pFace);
	SAFE_DELETE(m_pFlee);
	SAFE_DELETE(m_pEvade);
	SAFE_DELETE(m_pArrive);
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

Face* SmartAgent::GetFaceBehavior() const
{
	return m_pFace;
}

RadarSeek* SmartAgent::GetRadarSeekBehavior() const
{
	return m_pRadarSeek;
}

Flee* SmartAgent::GetFleeBehavior() const
{
	return m_pFlee;
}

Evade* SmartAgent::GetEvadeBehavior() const
{
	return m_pEvade;
}

Arrive* SmartAgent::GetArriveBehavior() const
{
	return m_pArrive;
}

// bool SmartAgent::HasLineOfSight(const Elite::Vector2& pos) const
// {
// 	ClosestRaycastCallback closestCallback = ClosestRaycastCallback();
// 	PHYSICSWORLD->Raycast(&closestCallback, GetPosition(), pos);
//
// 	return closestCallback.GetClosestPoint().vHitShapePoints.empty();
// }
