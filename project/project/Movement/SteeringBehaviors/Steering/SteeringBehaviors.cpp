//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"

using namespace Elite;

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ EXAMINTERFACE->NavMesh_GetClosestPathPoint(m_Target.Position) };
	const Vector2 dir{ targetPos - currentPos };

	steering.LinearVelocity = dir.GetNormalized() * pAgent->GetMaxLinearSpeed();
	
	if (DistanceSquared(currentPos, m_Target.Position) < m_TargetRadius * m_TargetRadius)
	{
		steering.IsValid = false;
		return steering;
	}

	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, { 1.0f, 1.0f, 0 }, EXAMINTERFACE->NextDepthSlice());
	}

	return steering;
}

//RADARSEEK
//****
SteeringOutput RadarSeek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ EXAMINTERFACE->NavMesh_GetClosestPathPoint(m_Target.Position) };
	const Vector2 dir{ targetPos - currentPos };

	steering.AngularVelocity = pAgent->GetMaxAngularSpeed();
	steering.LinearVelocity = dir.GetNormalized() * pAgent->GetMaxLinearSpeed();
	
	if (DistanceSquared(currentPos, m_Target.Position) < m_TargetRadius * m_TargetRadius)
	{
		steering.IsValid = false;
		return steering;
	}

	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, { 1.0f, 1.0f, 0 }, EXAMINTERFACE->NextDepthSlice());
	}

	return steering;
}

//FLEE
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	const Vector2 currentPos{ pAgent->GetPosition() };
	const Vector2 targetPos{ EXAMINTERFACE->NavMesh_GetClosestPathPoint(m_Target.Position) };
	const Vector2 dir{ targetPos - currentPos };

	steering.LinearVelocity = -dir.GetNormalized() * pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, { 1.0f, 1.0f, 0 }, EXAMINTERFACE->NextDepthSlice());
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
	
	const float distanceSq{ dir.MagnitudeSquared() };
	if (distanceSq < m_TargetRadius * m_TargetRadius)
	{
		steering.IsValid = false;
		return steering;
	}
	
	float speed{ pAgent->GetMaxLinearSpeed() };
	const float a{ distanceSq - m_TargetRadius * m_TargetRadius };
	const float b{ m_SlowRadius - m_TargetRadius };
	
	if (distanceSq < m_SlowRadius * m_SlowRadius)
	{
		speed *= (a / b);
	}

	steering.LinearVelocity = dir.GetNormalized() * speed;

	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, { 1.0f, 1.0f, 0 }, EXAMINTERFACE->NextDepthSlice());

		EXAMINTERFACE->Draw_Circle(currentPos, m_SlowRadius, { 1.0f, 0.0f, 1.0f }, EXAMINTERFACE->NextDepthSlice());
		EXAMINTERFACE->Draw_Circle(currentPos, m_TargetRadius, { 0.0f, 1.0f, 1.0f }, EXAMINTERFACE->NextDepthSlice());
	}
	return steering;
}

//Face
//****
SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	const Vector2 currentPos = pAgent->GetPosition();
	const Vector2 targetPos = m_Target.Position;
	const Vector2 dir = targetPos - currentPos;

	// Calculate the target angle
	float targetAngle = std::atan2f(dir.y, dir.x);

	// Ensure the target angle is in the range [0, 2pi)
	targetAngle = std::fmod(targetAngle + 2.0f * static_cast<float>(E_PI), 2.0f * static_cast<float>(E_PI));
	if (targetAngle < 0.0f)
	{
		targetAngle += 2.0f * static_cast<float>(E_PI);
	}

	// Calculate the angular difference between the current and target angles
	float angleDiff = targetAngle - pAgent->GetRotation();

	// Map the angle difference to the range [-pi, pi)
	angleDiff = std::fmod(angleDiff + static_cast<float>(E_PI), 2.0f * static_cast<float>(E_PI)) - static_cast<float>(E_PI);
	if (angleDiff < -static_cast<float>(E_PI))
	{
		angleDiff += 2.0f * static_cast<float>(E_PI);
	}
	else if (angleDiff >= static_cast<float>(E_PI))
	{
		angleDiff -= 2.0f * static_cast<float>(E_PI);
	}

	// prevent jittering	
	if (std::abs(angleDiff) < 0.1f)
	{
		steering.IsValid = false;
		return steering;
	}

	// Calculate the angular velocity
	steering.AngularVelocity = angleDiff / deltaT;

	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, {1.0f, 1.0f, 0}, EXAMINTERFACE->NextDepthSlice());
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
		steering.IsValid = false;
		if (pAgent->GetDebugRenderingEnabled())
		{
			EXAMINTERFACE->Draw_Circle(pAgent->GetPosition(), m_EvadeRadius, { 1.0f, 0.0f, 0.0f }, EXAMINTERFACE->NextDepthSlice());
		}
		return steering;
	}
	
	const float distance{std::sqrtf(distanceSq)};
	const float t { distance / pAgent->GetMaxLinearSpeed() };
	
	const Vector2 prediction{ m_Target.Position + m_Target.LinearVelocity * t };
	m_Target.Position = prediction;

	// Flee
	steering = Flee::CalculateSteering(deltaT, pAgent);
	
	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_SolidCircle(m_Target.Position, 0.7f, {0,0},{ 0.0f, 0.0f, 0.0f }, EXAMINTERFACE->NextDepthSlice());
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

	// Seek
	steering = Seek::CalculateSteering(deltaT, pAgent);
	
	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Point(m_Target.Position, 5.0f, { 1.0f, 0.0f, 0.0f }, EXAMINTERFACE->NextDepthSlice());
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
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, { 1.0f, 1.0f, 0 }, EXAMINTERFACE->NextDepthSlice());
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
		EXAMINTERFACE->Draw_Direction(currentPos, currentPos + steering.LinearVelocity, 5.0f, { 1.0f, 1.0f, 0 }, EXAMINTERFACE->NextDepthSlice());
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

	// Seek
	steering = Seek::CalculateSteering(deltaT, pAgent);
	
	if (pAgent->GetDebugRenderingEnabled())
	{
		EXAMINTERFACE->Draw_Segment(currentPos, targetPos, { 1.0f, 0.5f, 0 });
		EXAMINTERFACE->Draw_Circle(circleCenter, m_Radius, { 0.0f, 0.0f, 1.0f }, EXAMINTERFACE->NextDepthSlice());
		EXAMINTERFACE->Draw_Point(circleCenter, 5.0f, { 0.0f, 0.0f, 1.0f }, EXAMINTERFACE->NextDepthSlice());
	}
	
	return steering;
}
