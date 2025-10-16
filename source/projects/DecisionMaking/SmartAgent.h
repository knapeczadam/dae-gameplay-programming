/*=============================================================================*/
// SmartAgent.h: extention of SteeringAgent which has decision making capabilities
/*=============================================================================*/

#pragma once
#include "../Movement/SteeringBehaviors/SteeringAgent.h"
#include "../Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"

class SmartAgent : public SteeringAgent
{
public:
	//--- Constructor & Destructor ---
	SmartAgent(float radius = 1.0f);
	virtual ~SmartAgent();

	//--- Agent Functions ---
	void Update(float dt) override;
	void SetDecisionMaking(Elite::IDecisionMaking* decisionMakingStructure);

	Wander* GetWanderBehavior() const;
	Seek* GetSeekBehavior() const;

	bool HasLineOfSight(const Elite::Vector2& pos) const;

protected:
	Elite::IDecisionMaking* m_pDecisionMaking = nullptr;

	// Easy to access behaviors
	Wander* m_pWander = nullptr;
	Seek* m_pSeek = nullptr;
};

