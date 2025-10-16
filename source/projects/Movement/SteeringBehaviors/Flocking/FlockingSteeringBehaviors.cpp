#include "stdafx.h"
#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
// steer towards average position of neighbors
SteeringOutput Cohesion::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	const Elite::Vector2 avgNeighborPos{ m_pFlock->GetAverageNeighborPos() };
	m_Target = avgNeighborPos;	
    steering = Seek::CalculateSteering(deltaT, pAgent);
	return steering;
}

//*********************
//SEPARATION (FLOCKING)
// avoid neighbors that are within a small distance (neighborhood)
// Move away from neighbors with a speed that’s inversely proportional (y=1/x) to the distance to that neighbor. The closer a neighbor is, the more impact it should have on the output velocity.
SteeringOutput Separation::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	Elite::Vector2 velocity{};
	for (int idx{0}; idx < m_pFlock->GetNrOfNeighbors(); ++idx)
	{
		Elite::Vector2 direction{pAgent->GetPosition() - m_pFlock->GetNeighbors()[idx]->GetPosition()};
		const float distance{direction.MagnitudeSquared()};
		if (distance < 0.01f) continue;
		velocity += direction / distance;
	}
	steering.LinearVelocity = velocity.GetNormalized() * pAgent->GetMaxLinearSpeed();
	return steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
// steer towards average velocity of neighbors
SteeringOutput VelocityMatch::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering{};
	const Elite::Vector2 avgNeighborVelocity{ m_pFlock->GetAverageNeighborVelocity() };
	steering.LinearVelocity = avgNeighborVelocity;
	return steering;
}
