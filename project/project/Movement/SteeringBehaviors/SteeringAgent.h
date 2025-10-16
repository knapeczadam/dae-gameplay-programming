/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringAgent.h: basic agent using steering behaviors
/*=============================================================================*/
#ifndef STEERING_AGENT_H
#define STEERING_AGENT_H

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../../Shared/BaseAgent.h"
#include "SteeringHelpers.h"
class ISteeringBehavior;

class SteeringAgent : public BaseAgent
{
public:
	//--- Constructor & Destructor ---
	SteeringAgent() { std::cout << "SteeringAgent created!\n";}
	SteeringAgent(float radius) : BaseAgent(radius) {};
	virtual ~SteeringAgent() = default;

	//--- Agent Functions ---
	void Update(float dt) override;
	void Render(float dt) override;

	// float GetMaxLinearSpeed() const { return m_MaxLinearSpeed; }
	float GetMaxLinearSpeed() const { return EXAMINTERFACE->Agent_GetInfo().MaxLinearSpeed; }
	void SetMaxLinearSpeed(float maxLinSpeed) { m_MaxLinearSpeed = maxLinSpeed; }

	// float GetMaxAngularSpeed() const { return m_MaxAngularSpeed; }
	float GetMaxAngularSpeed() const { return EXAMINTERFACE->Agent_GetInfo().MaxAngularSpeed; }
	void SetMaxAngularSpeed(float maxAngSpeed) { m_MaxAngularSpeed = maxAngSpeed; }

	bool IsAutoOrienting() const { return m_AutoOrient; }
	void SetAutoOrient(bool autoOrient) { m_AutoOrient = autoOrient; }

	bool CanRun() const { return m_CanRun; }
	void CanRun(bool canRun) { m_CanRun = canRun; }

	Elite::Vector2 GetDirection() const { return GetLinearVelocity().GetNormalized(); }
	Elite::Vector2 GetPreviousPosition() const { return m_prevPos; }
	void SetPreviousPosition(Elite::Vector2 pos) { m_prevPos = pos; }

	virtual void SetSteeringBehavior(ISteeringBehavior* pBehavior) { m_pSteeringBehavior = pBehavior; }
	ISteeringBehavior* GetSteeringBehavior() const { return m_pSteeringBehavior; }

	void SetDebugRenderingEnabled(bool isEnabled) { m_RenderDebug = isEnabled; }
	bool GetDebugRenderingEnabled() const { return m_RenderDebug; }

	SteeringOutput GetSteeringOutput() const { return m_Steering; }

protected:
	//--- Datamembers ---
	ISteeringBehavior* m_pSteeringBehavior = nullptr;

	float          m_MaxLinearSpeed  = 10.f;
	float          m_MaxAngularSpeed = 10.f;
	bool           m_AutoOrient      = false;
	bool           m_CanRun          = false;
	bool           m_RenderDebug     = false;
	Elite::Vector2 m_prevPos         = {0,0 };
	SteeringOutput m_Steering        = {};
};
#endif