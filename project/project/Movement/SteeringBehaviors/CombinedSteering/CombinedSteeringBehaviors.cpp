#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors)
	:m_WeightedBehaviors(weightedBehaviors)
{
};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput blendedSteering = {};
	float totalWeight{ 0.f };

	//TODO: Calculate the weighted average steeringbehavior
	for (const auto& weightedBehavior : m_WeightedBehaviors)
	{
		SteeringOutput steering = weightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);
		blendedSteering.LinearVelocity += steering.LinearVelocity * weightedBehavior.weight;
		blendedSteering.AngularVelocity += steering.AngularVelocity * weightedBehavior.weight;

		totalWeight += weightedBehavior.weight;
	}

	if (totalWeight > 0)
	{
		const float scale{1.0f / totalWeight};
		blendedSteering *= scale;
		// blendedSteering.LinearVelocity /= totalWeight;
		// blendedSteering.AngularVelocity /= totalWeight;
	if (pAgent->GetDebugRenderingEnabled())
		EXAMINTERFACE->Draw_Direction(pAgent->GetPosition(), blendedSteering.LinearVelocity, 7, { 0, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
	}


	return blendedSteering;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	for (const auto& pBehavior : m_PriorityBehaviors)
	{
		steering = pBehavior->CalculateSteering(deltaT, pAgent);

		if (steering.IsValid)
			break;
	}

	//If non of the behavior return a valid output, last behavior is returned
	return steering;
}