#ifndef BASE_AGENT_H
#define BASE_AGENT_H

#include "GlobalInterface.h"

class BaseAgent
{
public:
	BaseAgent(float radius = 1.f);
	virtual ~BaseAgent();

	virtual void Update(float dt);
	virtual void Render(float dt);

	//Functions
	void TrimToWorld(float worldBounds, bool isWorldLooping = true) const;
	void TrimToWorld(const Elite::Vector2& bottomLeft, const Elite::Vector2& topRight, bool isWorldLooping = true) const;

	//Get - Set
	Elite::Vector2 GetPosition() const { return EXAMINTERFACE->Agent_GetInfo().Position ;}
	// void SetPosition(const Elite::Vector2& pos) const { m_pRigidBody->SetPosition(pos); }

	// float GetRotation() const { return Elite::ClampedAngle(m_pRigidBody->GetRotation().x);}
	float GetRotation() const { return Elite::ClampedAngle(EXAMINTERFACE->Agent_GetInfo().Orientation);}
	void SetRotation(float rot) { m_Rotation = rot; } 

	// Elite::Vector2 GetLinearVelocity() const { return m_pRigidBody->GetLinearVelocity(); }
	Elite::Vector2 GetLinearVelocity() const { return EXAMINTERFACE->Agent_GetInfo().LinearVelocity; }
	void SetLinearVelocity(const Elite::Vector2& linVel) { m_LinearVelocity = linVel; }

	// float GetAngularVelocity() const { return m_pRigidBody->GetAngularVelocity().x; }
	float GetAngularVelocity() const { return EXAMINTERFACE->Agent_GetInfo().AngularVelocity; }
	void SetAngularVelocity(float angVel) { m_AngularVelocity = angVel; }
	
	float GetMass() const { return m_Mass; }
	void SetMass(float mass) { m_Mass = mass; }

	// const Elite::Color& GetBodyColor() const { return m_BodyColor; }
	// void SetBodyColor(const Elite::Color& col) { m_BodyColor = col; }

	// Elite::RigidBodyUserData GetUserData() const { return m_pRigidBody->GetUserData(); }
	// void SetUserData(Elite::RigidBodyUserData userData) { m_pRigidBody->SetUserData(userData); }

	float GetRadius() const { return m_Radius; }

protected:
	// RigidBody* m_pRigidBody = nullptr;
	float m_Radius = 1.f;
	// Elite::Color m_BodyColor = { 1,1,0,1 };
private:
	float          m_Rotation        = 0.f;
	Elite::Vector2 m_LinearVelocity  = {0,0 };
	float          m_AngularVelocity = 0.f;
	float          m_Mass            = 1.f;

	//C++ make the class non-copyable
	BaseAgent(const BaseAgent&) {};
	BaseAgent& operator=(const BaseAgent&) {};

};
#endif