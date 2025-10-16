#pragma once
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include "framework/EliteAI/EliteGraphs/EliteInfluenceMap/EInfluenceMap.h"
class AntAgent : public SteeringAgent
{

public:
	AntAgent( Elite::InfluenceMap* pInfluenceMapFood, Elite::InfluenceMap* pInfluenceMapHome, float radius = 1.0f);
	virtual ~AntAgent();

	float GetSampleDistance() const { return m_sampleDistance; }
	void SetSampleDistance(float sampleDistance) { m_sampleDistance = sampleDistance; }

	float GetSampleAngle()const { return m_sampleAngle; }
	void SetSampleAngle(float sampleAngle) { m_sampleAngle = sampleAngle; }

	float GetInfluencePerSecond() const { return m_influencePerSecond; }
	void SetInfluencePerSecond(float influence) { m_influencePerSecond = influence; }

	void SetWanderAmount(float wanderPct);

	bool GetHasFood() const { return m_HasFood; }
	void SetHasFood(bool hasFood);

	void Update(float deltaTime);

private:
	Elite::InfluenceMap* m_pInfluenceMap_Home{ nullptr };
	Elite::InfluenceMap* m_pInfluenceMap_Food{ nullptr };
	Wander* m_pWander{ nullptr };
	Seek* m_pSeek{ nullptr };
	BlendedSteering* m_pBlendedSteering{ nullptr };

	bool m_HasFood{ false };

	float m_influencePerSecond{ 10.f };
	float m_sampleDistance{ 10.0f };
	float m_sampleAngle{ 45.f };

};