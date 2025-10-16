#include "stdafx.h"
#include "EGraphEditor.h"
#include <iostream>
#include "framework\EliteAI\EliteGraphs\EliteGraph\EGraphNode.h"
#include "framework\EliteAI\EliteGraphs\EliteGraph\EGraphConnection.h"
#include "framework\EliteAI\EliteGraphs\EliteGraph\EGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteGridGraph\EGridGraph.h"
#include "framework\EliteAI\EliteGraphs\EliteTerrainGridGraph\ETerrainGridGraph.h"

using namespace Elite;


bool GraphEditor::UpdateGraph(Graph* pGraph)
{
	bool hasGraphChanged = false;
	auto mouseMotionData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseMotion);


	if (INPUTMANAGER->IsMouseMoving())
	{
		m_MousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseMotionData.X, (float)mouseMotionData.Y });
		
		if (!m_mouseHasMoved && m_MousePos.DistanceSquared(m_initialMousePos) >= MOUSE_MOVE_TRESHOLD * MOUSE_MOVE_TRESHOLD)
		{
			m_mouseHasMoved = true;	
		}
	
	}
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		m_IsLeftMouseBtnPressed = false;
		if (m_mouseHasMoved)
		{
			m_SelectedNodeIdx = invalid_node_id;
			m_mouseHasMoved = false;
		}
	}

	// On left click:
	// - add connection
	// - select node
	// - add node
	if (INPUTMANAGER->IsMouseButtonDown(InputMouseButton::eLeft))
	{
		if (!m_IsLeftMouseBtnPressed)
		{
			m_IsLeftMouseBtnPressed = true;
			m_initialMousePos = m_MousePos;
			m_mouseHasMoved = false;
		}
		
		const GraphNode* pClickedNode = pGraph->GetNodeAtPosition(m_MousePos, 1.5f);
		if (pClickedNode != nullptr)
		{
			const int clickedNodeIdx = pClickedNode->GetId();
			if (m_SelectedNodeIdx != invalid_node_id)
			{

				if (pClickedNode != nullptr && m_SelectedNodeIdx != clickedNodeIdx)
				{
					if (pGraph->GetConnection(m_SelectedNodeIdx, clickedNodeIdx) == nullptr)
					{
						pGraph->AddConnection(new GraphConnection(m_SelectedNodeIdx, clickedNodeIdx));
						hasGraphChanged = true;
					}
				}

				m_SelectedNodeIdx = invalid_node_id;
			}
			else
			{
				m_SelectedNodeIdx = clickedNodeIdx;
			}
		}
		else
		{
			pGraph->AddNode(new GraphNode(m_MousePos));
			hasGraphChanged = true;
		}
	}

	// On right click:
	// - remove connection
	// - remove node
	if (INPUTMANAGER->IsMouseButtonDown(InputMouseButton::eRight))
	{
		GraphNode* clickedNode = pGraph->GetNodeAtPosition(m_MousePos);
		if (clickedNode != nullptr)
		{
			pGraph->RemoveNode(clickedNode->GetId());
			hasGraphChanged = true;
		}
		else
		{
			GraphConnection* clickedConnection = pGraph->GetConnectionAtPosition(m_MousePos, 4.f);
			if (clickedConnection)
			{
				pGraph->RemoveConnection(clickedConnection->GetFromNodeId(), clickedConnection->GetToNodeId());
				hasGraphChanged = true;
			}
		}
		m_SelectedNodeIdx = invalid_node_id;
	}

	// Update pClickedNode, edge and debug drawing positions
	if (m_SelectedNodeIdx != invalid_node_id)
	{
		const Elite::Vector2& nodePos = pGraph->GetNode(m_SelectedNodeIdx)->GetPosition();
		if (m_IsLeftMouseBtnPressed)
		{
			DEBUGRENDERER2D->DrawCircle(nodePos, DEFAULT_NODE_RADIUS, { 1,1,1 }, -1);
			if (m_mouseHasMoved)
			{
				pGraph->GetNode(m_SelectedNodeIdx)->SetPosition(m_MousePos);
			}
			hasGraphChanged = true;
		}

		if (!m_IsLeftMouseBtnPressed)
			DEBUGRENDERER2D->DrawSegment(nodePos, m_MousePos, DEFAULT_CONNECTION_COLOR, -1);
	}

	return hasGraphChanged;
}


bool GraphEditor::UpdateGraph(GridGraph* pGraph)
{
	bool hasGraphChanged = false;

	//Check if clicked on grid
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
		int idx = pGraph->GetNodeIdAtPosition(mousePos);

		if (idx != invalid_node_id)
		{
			pGraph->AddConnectionsToAdjacentCells(idx);
			hasGraphChanged = true;
		}
	}
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eRight))
	{
		auto mouseData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eRight);
		Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseData.X, (float)mouseData.Y });
		int idx = pGraph->GetNodeIdAtPosition(mousePos);

		if (idx != invalid_node_id)
		{
			pGraph->RemoveAllConnectionsWithNode(idx);
			hasGraphChanged = true;
		}
	}
	return hasGraphChanged;
}

bool GraphEditor::UpdateGraph(TerrainGridGraph* pGraph)
{
#pragma region UI
	//Extra Grid Terrain UI
	{
		//Setup
		int menuWidth = 115;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;
		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height / 2.0f));
		ImGui::Begin("Grid Editing", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::PushAllowKeyboardFocus(false);

		ImGui::Text("Terrain Type");
		ImGui::Indent();
		if (ImGui::Combo("", &m_SelectedTerrainType, "Ground\0Mud\0Water", 3))
		{

		}

		/*Spacing*/ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion


	//Check if clicked on grid
	auto mouseLeftData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
	auto mouseMotionData = INPUTMANAGER->GetMouseData(Elite::InputType::eMouseMotion);

	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		Vector2 mousePos = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ (float)mouseLeftData.X, (float)mouseLeftData.Y });
		int idx = pGraph->GetNodeIdAtPosition(mousePos);

		if (idx != invalid_node_id)
		{
			std::vector<TerrainType> terrainTypeVec{ TerrainType::Ground, TerrainType::Mud, TerrainType::Water };

			pGraph->SetNodeTerrainType(idx, terrainTypeVec[m_SelectedTerrainType]);
			switch (terrainTypeVec[m_SelectedTerrainType])
			{
			case TerrainType::Water:
				pGraph->RemoveAllConnectionsWithNode(idx);
				break;
			default:
				pGraph->RemoveAllConnectionsWithNode(idx);
				pGraph->AddConnectionsToAdjacentCells(idx);
				break;
			}
			return true;
		}
	}

	return false;
}


