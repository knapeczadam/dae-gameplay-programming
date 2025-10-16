//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"
#include <limits>

using namespace Elite;

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ m_Target.Position };
	const Vector2 dir{ targetPos - currentPos };
	steering.LinearVelocity = dir.GetNormalized() * pAgent->GetMaxLinearSpeed();
	const Vector2 face{ Vector2{} - currentPos };
	const float angle{ std::atan2f(face.y, face.x)};
	steering.AngularVelocity = (angle - pAgent->GetRotation()) * pAgent->GetMaxAngularSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, currentPos + steering.LinearVelocity, 5.0f,  { 1.0f, 1.0f, 0 });
	}

	return steering;
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ m_Target.Position };
	const Vector2 dir{ targetPos - currentPos };

	steering.LinearVelocity = -dir.GetNormalized() * pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, currentPos + steering.LinearVelocity, 5.0f,  { 1.0f, 1.0f, 0 });
	}

	return steering;
}

//ARRIVE
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ m_Target.Position };
	const Vector2 dir{ targetPos - currentPos };
	const float distance{ dir.Magnitude() };
	float speed{ pAgent->GetMaxLinearSpeed() };
	const float a{ distance - m_TargetRadius };
	const float b{ m_SlowRadius - m_TargetRadius };
	if (distance < m_SlowRadius)
	{
		speed *= (a / b);
	}

	steering.LinearVelocity = dir.GetNormalized() * speed;

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, currentPos + steering.LinearVelocity, 5.0f,  { 1.0f, 1.0f, 0 });

		DEBUGRENDERER2D->DrawCircle(currentPos, m_SlowRadius, { 0.0f, 0.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawCircle(currentPos, m_TargetRadius, { 0.0f, 1.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}
	return steering;
}

//Face
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ m_Target.Position };
	const Vector2 dir{ targetPos - currentPos };
	const float angle{ std::atan2f(dir.y, dir.x)};
	steering.AngularVelocity = (angle - pAgent->GetRotation()) * pAgent->GetMaxAngularSpeed();
	std::cout << steering.AngularVelocity << std::endl;

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, currentPos + steering.LinearVelocity, 5.0f,  { 1.0f, 1.0f, 0 });
	}
	return steering;
}

//Evade
//****
SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	const float distanceSq{ (m_Target.Position - pAgent->GetPosition()).MagnitudeSquared() };

	if (distanceSq > m_EvadeRadius * m_EvadeRadius)
	{
		SteeringOutput output{};
		output.IsValid = false;
		if (pAgent->GetDebugRenderingEnabled())
		{
			DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRadius, { 1.0f, 0.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
		}
		return output;
	}
	const float distance{std::sqrtf(distanceSq)};
	const float t { distance / pAgent->GetMaxLinearSpeed() };
	
	const Vector2 prediction{ m_Target.Position + m_Target.LinearVelocity * t };
	m_Target.Position = prediction;
	steering = Flee::CalculateSteering(deltaT, pAgent);
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawPoint(m_Target.Position, 5.0f, { 1.0f, 0.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}
	return steering;
}

//Pursuit
//****
SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	const float distance{ (m_Target.Position - pAgent->GetPosition()).Magnitude() };
	const float t { distance / pAgent->GetMaxLinearSpeed() };
	const Vector2 prediction{ m_Target.Position + m_Target.LinearVelocity * t };
	m_Target.Position = prediction;
	steering = Seek::CalculateSteering(deltaT, pAgent);
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawPoint(m_Target.Position, 5.0f, { 1.0f, 0.0f, 0.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}
	return steering;
}

//Hide
//****
SteeringOutput Hide::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ m_Target.Position };
	const Vector2 dir{ targetPos - currentPos };
	const float distance{ dir.Magnitude() };
	float speed{ pAgent->GetMaxLinearSpeed() };

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, currentPos + steering.LinearVelocity, 5.0f,  { 1.0f, 1.0f, 0 });
	}
	return steering;
}

//AvoidObstacle
//****
SteeringOutput AvoidObstacle::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ m_Target.Position };
	const Vector2 dir{ targetPos - currentPos };
	const float distance{ dir.Magnitude() };
	float speed{ pAgent->GetMaxLinearSpeed() };

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, currentPos + steering.LinearVelocity, 5.0f,  { 1.0f, 1.0f, 0 });
	}
	return steering;
}
//Wander
//****
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 currentDir{ pAgent->GetDirection() };
	const Vector2 circleCenter{currentPos + currentDir * m_OffsetDistance};
	m_WanderAngle += randomFloat(-m_MaxAngleChange / 2.0f, m_MaxAngleChange / 2.0f);
	const Vector2 targetPos{circleCenter + Vector2{std::cosf(m_WanderAngle), std::sinf(m_WanderAngle)} * m_Radius};
	m_Target.Position = targetPos;
	
	steering = Seek::CalculateSteering(deltaT, pAgent);
	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawSegment(currentPos, targetPos, { 1.0f, 0.5f, 0 });
		DEBUGRENDERER2D->DrawCircle(circleCenter, m_Radius, { 0.0f, 0.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawPoint(circleCenter, 5.0f, { 0.0f, 0.0f, 1.0f }, DEBUGRENDERER2D->NextDepthSlice());
	}
	return steering;
}
