#include "stdafx.h"
#include "SteeringAgent.h"
#include "Steering/SteeringBehaviors.h"

void SteeringAgent::Update(float dt)
{
	if(m_pSteeringBehavior)
	{
		auto output = m_pSteeringBehavior->CalculateSteering(dt, this);

		//Linear Movement
		//***************
		auto linVel = GetLinearVelocity();
		auto steeringForce = output.LinearVelocity - linVel;
		auto acceleration = steeringForce / GetMass();		

		if(m_RenderDebug)
		{
			EXAMINTERFACE->Draw_Direction(GetPosition(), acceleration, acceleration.Magnitude(), { 0, 1, 1 }, EXAMINTERFACE->NextDepthSlice());
			EXAMINTERFACE->Draw_Direction(GetPosition(), linVel, linVel.Magnitude(), { 1, 0, 1 }, EXAMINTERFACE->NextDepthSlice());
		}
		SetLinearVelocity(linVel + (acceleration*dt));

		//Angular Movement
		//****************
		if(m_AutoOrient)
		{
			auto desiredOrientation = Elite::VectorToOrientation(GetLinearVelocity());
			SetRotation(desiredOrientation);
		}
		else
		{
			if (output.AngularVelocity > m_MaxAngularSpeed)
				output.AngularVelocity = m_MaxAngularSpeed;
			SetAngularVelocity(output.AngularVelocity);
		}

		m_Steering.AngularVelocity = output.AngularVelocity;
		m_Steering.LinearVelocity  = output.LinearVelocity;
		m_Steering.IsValid         = output.IsValid;
	}
}

void SteeringAgent::Render(float dt)
{
	//Use Default Agent Rendering
	BaseAgent::Render(dt);
}