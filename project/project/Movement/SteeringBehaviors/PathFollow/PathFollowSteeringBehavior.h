#pragma once

#include "../Steering/SteeringBehaviors.h"

class PathFollow : public ISteeringBehavior
{
public:

	PathFollow();
	virtual ~PathFollow();
	void SetPath(std::vector<Elite::Vector2>& path);
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;

	bool HasArrived() const;

private:
	Seek* m_pSeek = nullptr;
	Arrive* m_pArrive = nullptr;
	ISteeringBehavior* m_pCurrentSteering = nullptr;
	std::vector<Elite::Vector2> m_pathVec{};
	int m_currentPathIndex;
	void GotoNextPathpoint();
};