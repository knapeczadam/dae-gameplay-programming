#pragma once
#include "Exam_HelperStructs.h"

//SteeringParams (alias TargetData)
struct SteeringParams //Also used as Target for SteeringBehaviors
{
	Elite::Vector2 Position;
	float Orientation;

	Elite::Vector2 LinearVelocity;
	float AngularVelocity;

	SteeringParams(Elite::Vector2 position = Elite::ZeroVector2, float orientation = 0.f, 
		Elite::Vector2 linearVel = Elite::ZeroVector2, float angularVel = 0.f) :
		Position(position),
		Orientation(orientation),
		LinearVelocity(linearVel),
		AngularVelocity(angularVel) {}

#pragma region Functions
	void Clear()
	{
		Position = Elite::ZeroVector2;
		LinearVelocity = Elite::ZeroVector2;

		Orientation = 0.f;
		AngularVelocity = 0.f;
	}
#pragma endregion

#pragma region Operator Overloads
	SteeringParams(const SteeringParams & other)
	{
		Position = other.Position;
		Orientation = other.Orientation;
		LinearVelocity = other.LinearVelocity;
		AngularVelocity = other.AngularVelocity;
	}

	SteeringParams& operator=(const SteeringParams& other)
	{
		Position = other.Position;
		Orientation = other.Orientation;
		LinearVelocity = other.LinearVelocity;
		AngularVelocity = other.AngularVelocity;

		return *this;
	}

	bool operator==(const SteeringParams& other) const
	{
		return Position == other.Position && Orientation == other.Orientation && LinearVelocity == other.LinearVelocity && AngularVelocity == other.AngularVelocity;
	}

	bool operator!=(const SteeringParams& other) const
	{
		return Position != other.Position || Orientation != other.Orientation || LinearVelocity != other.LinearVelocity || AngularVelocity != other.AngularVelocity;
	}
#pragma endregion

};
using TargetData = SteeringParams; //Alias for SteeringBehavior usage (Bit clearer in its context ;) )

//SteeringOutput
struct SteeringOutput
{
	Elite::Vector2 LinearVelocity  = {0.f,0.f };
	float          AngularVelocity = 0.f;
	bool           IsValid         = true;

	SteeringOutput(Elite::Vector2 linVel = { 0.f,0.f }, float angVel = 0.f, bool isValid = true)
	{
		LinearVelocity  = linVel;
		AngularVelocity = angVel;
		IsValid         = isValid;
	}

	SteeringPlugin_Output AsSteeringPluginOutput() const
	{
		SteeringPlugin_Output steeringPluginOutput;
		steeringPluginOutput.LinearVelocity  = LinearVelocity;  
		steeringPluginOutput.AngularVelocity = AngularVelocity; //Rotate your character to inspect the world while walking
		steeringPluginOutput.AutoOrient      = true;
		steeringPluginOutput.RunMode         = false;
		return steeringPluginOutput;
	}

	SteeringOutput& operator=(const SteeringOutput& other)
	{
		LinearVelocity = other.LinearVelocity;
		AngularVelocity = other.AngularVelocity;
		IsValid = other.IsValid;

		return *this;
	}

	SteeringOutput& operator+(const SteeringOutput& other)
	{
		LinearVelocity += other.LinearVelocity;
		AngularVelocity += other.AngularVelocity;

		return *this;
	}

	SteeringOutput& operator*=(const SteeringOutput& other)
	{
		LinearVelocity = LinearVelocity * other.LinearVelocity;
		AngularVelocity = AngularVelocity * other.AngularVelocity;

		return *this;
	}

	SteeringOutput& operator*=(float f)
	{
		LinearVelocity = f * LinearVelocity;
		AngularVelocity = f * AngularVelocity;

		return *this;
	}

	SteeringOutput& operator/=(float f)
	{
		LinearVelocity = LinearVelocity / f;
		AngularVelocity = AngularVelocity / f;

		return *this;
	}
};

//=== TEMPORARILY ADDED HERE - IS PART OF COMBINED STEERING! ===
struct Goal
{
	//Four separate controllable channels
	Elite::Vector2 Position = Elite::ZeroVector2;
	bool PositionSet = false;

	Goal() = default;

	//Reset Goal's Channels
	void Clear()
	{
		Position = Elite::ZeroVector2;
		PositionSet = false;
	}

	//Update Goal
	void UpdateGoal(const Goal& goal)
	{
		if (goal.PositionSet)
		{
			Position = goal.Position;
			PositionSet = true;
		}
	}

	//Merge Goal
	bool CanMergeGoal(const Goal& goal) const
	{
		return !(PositionSet && goal.PositionSet);
	}
};