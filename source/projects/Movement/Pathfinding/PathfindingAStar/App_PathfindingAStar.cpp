//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_PathfindingAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAstar.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EBFS.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

using namespace Elite;

//Destructor
App_PathfindingAStar::~App_PathfindingAStar()
{
	SAFE_DELETE(m_pTerrainGraph);

	SAFE_DELETE(m_pAgent);
	SAFE_DELETE(m_pPathFollowBehavior);
}

//Functions
void App_PathfindingAStar::Start()
{
	//Set Camera
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(39.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(73.0f, 35.0f));

	//Create Graph
	MakeGridGraph();

	//Create Agent
	m_pPathFollowBehavior = new PathFollow();
	m_pAgent = new SteeringAgent(2.f);
	m_pAgent->SetSteeringBehavior(m_pPathFollowBehavior);
	m_pAgent->SetMass(0.1f);
	m_pAgent->SetMaxLinearSpeed(20.f);
	m_pAgent->SetMaxAngularSpeed(90.f);
	m_pAgent->SetAutoOrient(true);

	//Setup default start path
	m_startPathId = 44;
	m_endPathId = 88;
	CalculatePath();

}

void App_PathfindingAStar::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	//INPUT
	bool const middleMousePressed = INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle);
	if (middleMousePressed)
	{
		MouseData mouseData = { INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eMiddle) };
		Elite::Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });

		//Find closest node to click pos
		int closestNode = m_pTerrainGraph->GetNodeIdAtPosition(mousePos);
		if (m_StartSelected)
		{
			m_startPathId = closestNode;
			CalculatePath();
		}
		else
		{
			m_endPathId = closestNode;
			CalculatePath();
		}
	}
	//IMGUI
	UpdateImGui();

	//UPDATE/CHECK GRID HAS CHANGED
	if (m_GraphEditor.UpdateGraph(m_pTerrainGraph))
	{
		CalculatePath();
	}

	m_pAgent->Update(deltaTime);
}

void App_PathfindingAStar::Render(float deltaTime) const
{
	UNREFERENCED_PARAMETER(deltaTime);
	//Render grid
	m_GraphRenderer.RenderGraph(
		m_pTerrainGraph,
		m_bDrawGrid,
		m_bDrawNodeNumbers,
		m_bDrawConnections,
		m_bDrawConnectionsCosts
	);

	//render path below if applicable
	if (m_vPath.size() > 0)
	{
		m_GraphRenderer.HighlightNodes(m_pTerrainGraph, m_vPath);
	}

	//Render start node on top if applicable
	if (m_startPathId != invalid_node_id)
	{
		m_GraphRenderer.HighlightNodes(m_pTerrainGraph, { m_pTerrainGraph->GetNode(m_startPathId) }, START_NODE_COLOR);
	}

	//Render end node on top if applicable
	if (m_endPathId != invalid_node_id)
	{
		m_GraphRenderer.HighlightNodes(m_pTerrainGraph, { m_pTerrainGraph->GetNode(m_endPathId) }, END_NODE_COLOR);
	}



	m_pAgent->Render(deltaTime);
}

void App_PathfindingAStar::MakeGridGraph()
{
	m_pTerrainGraph = new TerrainGridGraph(COLUMNS, ROWS, m_SizeCell, false, false, static_cast<float>(m_SizeCell), m_SizeCell* 1.5f);

	//Setup default terrain
	m_pTerrainGraph->SetNodeTerrainType(86, TerrainType::Water);
	m_pTerrainGraph->SetNodeTerrainType(66, TerrainType::Water);
	m_pTerrainGraph->SetNodeTerrainType(67, TerrainType::Water);
	m_pTerrainGraph->SetNodeTerrainType(47, TerrainType::Water);
	m_pTerrainGraph->RemoveAllConnectionsWithNode(86);
	m_pTerrainGraph->RemoveAllConnectionsWithNode(66);
	m_pTerrainGraph->RemoveAllConnectionsWithNode(67);
	m_pTerrainGraph->RemoveAllConnectionsWithNode(47);
}

void App_PathfindingAStar::UpdateImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int menuWidth = 115;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: target");
		ImGui::Text("RMB: start");
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		ImGui::Text("A* Pathfinding");
		ImGui::Spacing();

		ImGui::Text("Middle Mouse");
		ImGui::Text("controls");
		std::string buttonText{ "" };
		if (m_StartSelected)
			buttonText += "Start Node";
		else
			buttonText += "End Node";

		if (ImGui::Button(buttonText.c_str()))
		{
			m_StartSelected = !m_StartSelected;
		}

		ImGui::Checkbox("Grid", &m_bDrawGrid);
		ImGui::Checkbox("NodeNumbers", &m_bDrawNodeNumbers);
		ImGui::Checkbox("Connections", &m_bDrawConnections);
		ImGui::Checkbox("Connections Costs", &m_bDrawConnectionsCosts);
		if (ImGui::Combo("", &m_SelectedHeuristic, "Manhattan\0Euclidean\0SqEuclidean\0Octile\0Chebyshev", 4))
		{
			switch (m_SelectedHeuristic)
			{
			case 0:
				m_heuristicFunction = HeuristicFunctions::Manhattan;
				break;
			case 1:
				m_heuristicFunction = HeuristicFunctions::Euclidean;
				break;
			case 2:
				m_heuristicFunction = HeuristicFunctions::SqEuclidean;
				break;
			case 3:
				m_heuristicFunction = HeuristicFunctions::Octile;
				break;
			default:
			case 4:
				m_heuristicFunction = HeuristicFunctions::Chebyshev;
				break;
			}
		}
		ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_PathfindingAStar::CalculatePath()
{
	//Check if valid start and end node exist
	if (m_startPathId != invalid_node_id
		&& m_endPathId != invalid_node_id
		&& m_startPathId != m_endPathId)
	{
		//Select (uncomment) BFS Pathfinding or A* Pathfinding
		// auto pathfinder = BFS(m_pTerrainGraph);
		auto pathfinder = AStar(m_pTerrainGraph, m_heuristicFunction);
		auto startNode = m_pTerrainGraph->GetNode(m_startPathId);
		auto endNode = m_pTerrainGraph->GetNode(m_endPathId);

		m_vPath = pathfinder.FindPath(startNode, endNode);
		std::cout << "New path calculated using " << typeid(pathfinder).name() << std::endl;
		UpdateAgentPath(m_vPath);
	}
	else
	{
		std::cout << "No valid start and end node..." << std::endl;
		m_vPath.clear();
	}
}

void App_PathfindingAStar::UpdateAgentPath(const std::vector<Elite::GraphNode*>& path)
{

	std::vector<Vector2> pathPositions{};
	pathPositions.reserve(path.size());
	for (auto& pNode : path)
	{
		pathPositions.emplace_back(pNode->GetPosition());
	}

	m_pPathFollowBehavior->SetPath(pathPositions);
	if (pathPositions.size() > 0)
	{
		m_pAgent->SetPosition(pathPositions[0]);
	}


}
