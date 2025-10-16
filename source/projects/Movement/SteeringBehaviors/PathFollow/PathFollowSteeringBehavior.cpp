#include "stdafx.h"

#include "PathFollowSteeringBehavior.h"
#include "../SteeringAgent.h"

PathFollow::PathFollow()
{
	m_pSeek = new Seek();
	m_pArrive = new Arrive();
	m_pArrive->SetTargetRadius(.5f);
	m_pArrive->SetSlowRadius(5.f);
}

PathFollow::~PathFollow()
{
	SAFE_DELETE(m_pArrive);
	SAFE_DELETE(m_pSeek);
}

void PathFollow::SetPath(std::vector<Elite::Vector2>& path)
{
	m_pathVec = path;  
	
	m_currentPathIndex = -1;
	GotoNextPathpoint();
}

SteeringOutput PathFollow::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{

	if (m_currentPathIndex < m_pathVec.size())
	{
		float agentRadius = pAgent->GetRadius();

		if (Elite::DistanceSquared(pAgent->GetPosition(), m_pathVec[m_currentPathIndex]) < agentRadius * agentRadius)
		{
			//Reached point of the path
			GotoNextPathpoint();
		}
	}

	if (m_pCurrentSteering != nullptr)
	{
		return m_pCurrentSteering->CalculateSteering(deltaT, pAgent);
	}
	return SteeringOutput{};
}

bool PathFollow::HasArrived() const
{
	return m_currentPathIndex >= m_pathVec.size();
}

void PathFollow::GotoNextPathpoint()
{
	++m_currentPathIndex;
	if (m_currentPathIndex >= m_pathVec.size()) return;

	if (m_currentPathIndex == m_pathVec.size() -1)
	{
		//We have reached the last node
		m_pArrive->SetTarget(m_pathVec[m_currentPathIndex]);
		m_pCurrentSteering = m_pArrive;
	}
	else
	{
		//Move to the next node
		m_pSeek->SetTarget(m_pathVec[m_currentPathIndex]);
		m_pCurrentSteering = m_pSeek;
	}
}
