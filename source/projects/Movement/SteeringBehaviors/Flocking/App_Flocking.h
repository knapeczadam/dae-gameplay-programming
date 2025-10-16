#pragma once
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "../Steering/SteeringBehaviors.h"

class SteeringAgent;
class Flock;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_Flocking final : public IApp
{
public:
	//Constructor & Destructor
	App_Flocking() = default;
	~App_Flocking() override;
	App_Flocking(const App_Flocking&) = delete;
	App_Flocking& operator=(const App_Flocking&) = delete;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	TargetData m_MouseTarget {};
	
	bool       m_UseMouseTarget       {true};
	bool       m_VisualizeMouseTarget {true};

	float m_WorldSize {500.f};
	int   m_FlockSize {100};

	Flock*         m_pFlock        {nullptr};
	SteeringAgent* m_pAgentToEvade {nullptr};
	Wander*        m_pWander       {nullptr};

	//C++ make the class non-copyable
};