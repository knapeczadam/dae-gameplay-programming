/*=============================================================================*/
// Copyright 2023-2024 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#pragma once

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../SteeringHelpers.h"
class SteeringAgent;
class Obstacle;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	~Seek() override = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};


///////////////////////////////////////
//FLEE
//****
class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	~Flee() override = default;

	//Flee Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

///////////////////////////////////////
//ARRIVE
//****
class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	~Arrive() override = default;

	//Arrive Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
	void SetTargetRadius(float radius) { m_TargetRadius = radius; }
	void SetSlowRadius(float radius) { m_SlowRadius = radius;}
private:
	float m_SlowRadius = 15.0f;
	float m_TargetRadius = 3.0f;
};

///////////////////////////////////////
//Face
//****
class Face : public ISteeringBehavior
{
public:
	Face() = default;
	~Face() override = default;

	//Face Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
};

///////////////////////////////////////
//Pursuit
//****
class Pursuit : public Seek
{
public:
	Pursuit() = default;
	~Pursuit() override = default;

	//Pursuit Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
};

///////////////////////////////////////
//Evade
//****
class Evade : public Flee
{
public:
	Evade(float evadeRadius = 15.f) : m_EvadeRadius(evadeRadius) {}
	~Evade() override = default;

	//Evade Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
	float m_EvadeRadius = 15.f;
};

///////////////////////////////////////
//Hide
//****
class Hide : public ISteeringBehavior
{
public:
	Hide() = default;
	~Hide() override = default;

	//Hide Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
};

///////////////////////////////////////
//AvoidObstacle
//****
class AvoidObstacle : public ISteeringBehavior
{
public:
	AvoidObstacle() = default;
	~AvoidObstacle() override = default;

	//AvoidObstacle Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
private:
};

///////////////////////////////////////
//AvoidObstacle
//****
class Wander : public Seek
{
public:
	Wander() = default;
	~Wander() override = default;

	//Wander Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }

protected:
	float m_OffsetDistance = 6.f; //Offset (Agent Direction)
	float m_Radius = 4.f; //Circle Radius
	float m_MaxAngleChange = Elite::ToRadians(45); //Max Wander Angle change per frame
	float m_WanderAngle = 0.f; //Internal
private:
};
