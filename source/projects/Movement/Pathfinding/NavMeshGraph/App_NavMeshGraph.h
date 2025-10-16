#pragma once
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EPathSmoothing.h"

//Forward declarations
class NavigationColliderElement;
class SteeringAgent;
class PathFollow;

namespace Elite
{
	class NavGraph;
	class GraphRenderer;
}


//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_NavMeshGraph final : public IApp
{
public:
	//Constructor & Destructor
	App_NavMeshGraph() = default;
	virtual ~App_NavMeshGraph();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	// --Agents--
	SteeringAgent* m_pAgent = nullptr;
	PathFollow* m_pPathFollow = nullptr;
	TargetData m_Target = {};
	float m_AgentRadius = 1.0f;
	float m_AgentSpeed = 16.0f;

	// --Level--
	std::vector<NavigationColliderElement*> m_vNavigationColliders = {};

	// --Pathfinder--
	std::vector<Elite::Vector2> m_vPath;

	// --Graph--
	Elite::NavGraph* m_pNavGraph = nullptr;
	Elite::GraphRenderer* m_pGraphRenderer = nullptr;

	// --Debug drawing information--
	std::vector<Elite::Portal> m_Portals;
	std::vector<Elite::Vector2> m_DebugNodePositions;
	static bool sShowPolygon;
	static bool sShowGraph;
	static bool sDrawPortals;
	static bool sDrawFinalPath;
	static bool sDrawNonOptimisedPath;

	void UpdateImGui();
private:
	//C++ make the class non-copyable
	App_NavMeshGraph(const App_NavMeshGraph&) = delete;
	App_NavMeshGraph& operator=(const App_NavMeshGraph&) = delete;
};
