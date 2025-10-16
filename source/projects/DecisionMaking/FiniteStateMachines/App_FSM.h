#pragma once
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EPathSmoothing.h"
#include "projects/DecisionMaking/FiniteStateMachines/StatesAndTransitions.h"

//Forward declarations
class NavigationColliderElement;
class SteeringAgent;
class PathFollow;

class PatrolState;
class ChaseState;
class SearchState;

class IsTargetVisible;
class IsTargetNotVisible;
class IsSearchingTooLong;


namespace Elite
{
	class NavGraph;
	struct DebugHelper;
	
}


//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_FSM final : public IApp
{
public:
public:
	//Constructor & Destructor
	App_FSM() = default;
	virtual ~App_FSM();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	Elite::Blackboard* CreateBlackboard(SmartAgent* smartAgent);

	//Datamembers
	// --AI--
	// FSM TODO:
	PatrolState* m_pPatrolState = nullptr;
	ChaseState*  m_pChaseState  = nullptr;
	SearchState* m_pSearchState = nullptr;

	IsTargetVisible*    m_pIsTargetVisible    = nullptr;
	IsTargetNotVisible* m_pIsTargetNotVisible = nullptr;
	IsSearchingTooLong* m_pIsSearchingTooLong = nullptr;

	FiniteStateMachine* m_pFSM = nullptr;

	Blackboard* m_pBlackboard = nullptr;

	// --Agents--
	SteeringAgent* m_pPlayer           = nullptr;
	SmartAgent*    m_pGuard            = nullptr;
	PathFollow*    m_pPlayerPathFollow = nullptr;
	PathFollow*    m_pGuardPathFollow  = nullptr;
	TargetData     m_Target            = {};
	float          m_AgentRadius       = 1.0f;
	float          m_PlayerSpeed       = 16.0f;
	float          m_GuardSpeed        = 8.0f;
	float          m_DetectionRadius   = 10.0f;
	float          m_MaxSearchTime     = 5.0f;
	float          m_AccuSearchTime    = 0.0f;
	float          m_GuardViewAngle    = 90.0f;
	float          m_GuardViewDistance = 10.0f;
	DebugHelper    m_DebugHelper       = {};

	// --Level--
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};

	// --Pathfinder--
	std::vector<Elite::Vector2> m_vPath      = {};
	std::vector<Elite::Vector2> m_PatrolPath = {};

	// --Graph--
	Elite::NavGraph* m_pNavGraph = nullptr;

	// --Debug drawing information--
	static bool sShowPolygon;
	static bool sDrawPlayerPath;
	static bool sDrawPatrolPath;
	static bool sDrawDetectionRadius;
	static bool sDrawAgentGuardSegment;
	static bool sDrawGuardViewCone;
	static bool sDrawGuardForwardVelocity;
	static bool sDrawPlayerLastSeenPos;

	void UpdateImGui();
private:
	//C++ make the class non-copyable
	App_FSM(const App_FSM&) = delete;
	App_FSM& operator=(const App_FSM&) = delete;
};
