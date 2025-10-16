#include "stdafx.h"
#include "SandboxAgent.h"

using namespace Elite;

SandboxAgent::SandboxAgent(): BaseAgent()
{
	m_Target = GetPosition();
}

void SandboxAgent::Update(float dt)
{
	//TODO: set linear velocity towards m_Target
	const Vector2 pos{ GetPosition() };
	const Vector2 dir{ m_Target - pos };
	const float speed{ 10.0f };
	const Vector2 vel{ dir.GetNormalized() * speed };
	SetLinearVelocity(vel);

	DEBUGRENDERER2D->DrawSegment(pos, m_Target, { 0, 1.0f, 0 });
	//Orientation
	AutoOrient();
}

void SandboxAgent::Render(float dt)
{
	BaseAgent::Render(dt); //Default Agent Rendering
}

void SandboxAgent::AutoOrient()
{
	//Determine angle based on direction
	Vector2 velocity = GetLinearVelocity();
	if (velocity.MagnitudeSquared() > 0)
	{
		SetRotation(VectorToOrientation(velocity));
	}
}