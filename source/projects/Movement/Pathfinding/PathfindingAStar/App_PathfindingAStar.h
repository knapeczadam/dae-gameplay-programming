#pragma once
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGridGraph.h"
#include "framework/EliteAI/EliteGraphs/EliteTerrainGridGraph/ETerrainGraphNode.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EHeuristic.h"

//Forward declerations
class SteeringAgent;
class PathFollow;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_PathfindingAStar final : public IApp
{
public:
	//Constructor & Destructor
	App_PathfindingAStar() = default;
	virtual ~App_PathfindingAStar();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	const bool ALLOW_DIAGONAL_MOVEMENT = true;
	Elite::Vector2 m_StartPosition = Elite::ZeroVector2;
	Elite::Vector2 m_TargetPosition = Elite::ZeroVector2;

	//Grid datamembers
	static const int COLUMNS = 20;
	static const int ROWS = 10;
	unsigned int m_SizeCell = 10;
	Elite::TerrainGridGraph* m_pTerrainGraph;

	//Pathfinding datamembers
	int m_startPathId = invalid_node_id;
	int m_endPathId = invalid_node_id;
	std::vector<Elite::GraphNode*> m_vPath;

	//Editor and Visualisation
	Elite::GraphEditor m_GraphEditor{};
	Elite::GraphRenderer m_GraphRenderer{};

	//Debug rendering information
	bool m_bDrawGrid = true;
	bool m_bDrawNodeNumbers = false;
	bool m_bDrawConnections = false;
	bool m_bDrawConnectionsCosts = false;
	bool m_StartSelected = true;
	int m_SelectedHeuristic = 4;
	Elite::Heuristic m_heuristicFunction = Elite::HeuristicFunctions::Chebyshev;

	//Steering agent 
	SteeringAgent* m_pAgent = nullptr;
	PathFollow* m_pPathFollowBehavior = nullptr;


	//Functions
	void MakeGridGraph();
	void UpdateImGui();
	void CalculatePath();
	void UpdateAgentPath(const std::vector<Elite::GraphNode*>& path);

	//C++ make the class non-copyable
	App_PathfindingAStar(const App_PathfindingAStar&) = delete;
	App_PathfindingAStar& operator=(const App_PathfindingAStar&) = delete;
};
