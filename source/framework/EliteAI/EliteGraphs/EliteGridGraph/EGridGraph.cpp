#include "stdafx.h"

#include "../EliteGraph/EGraph.h"
#include "EGridGraph.h"
#include "../EliteGraph/EGraphEnums.h"
#include "../EliteGraph/EGraphConnection.h"
#include "EConnectionCostCalculator.h"

using namespace Elite;

GridGraph::GridGraph(
	int columns,
	int rows,
	int cellSize,
	bool isDirectionalGraph,
	bool isConnectedDiagonally,
	float costStraight /* = 1.f*/,
	float costDiagonal /* = 1.5f */,
	GraphNodeFactory* pFactory, ConnectionCostCalculator* pCostCalculator)
	: Graph(isDirectionalGraph, pFactory)
	, m_NrOfColumns(columns)
	, m_NrOfRows(rows)
	, m_CellSize(cellSize)
	, m_IsConnectedDiagonally(isConnectedDiagonally)
	, m_DefaultCostStraight(costStraight)
	, m_DefaultCostDiagonal(costDiagonal)
	, m_pCostCalculator(pCostCalculator)
{
	InitializeGrid();
}

GridGraph::~GridGraph()
{
	SAFE_DELETE(m_pCostCalculator);
}

void GridGraph::InitializeGrid()
{
	// Create all nodes
	for (auto r = 0; r < m_NrOfRows; ++r)
	{
		for (auto c = 0; c < m_NrOfColumns; ++c)
		{
			int idx = GetNodeId(c, r);
			AddNodeAtIndex(CreateNode(GetNodePos(idx)), idx);
		}
	}

	// Create connections in each valid direction on each node
	for (auto r = 0; r < m_NrOfRows; ++r)
	{
		for (auto c = 0; c < m_NrOfColumns; ++c)
		{
			AddConnectionsToAdjacentCells(c, r);
		}
	}
}

void Elite::GridGraph::AddConnectionsInDirections(int idx, int col, int row, std::vector<Elite::Vector2> directions)
{
	for (auto d : directions)
	{
		int neighborCol = col + (int)d.x;
		int neighborRow = row + (int)d.y;

		if (IsWithinBounds(neighborCol, neighborRow))
		{
			int neighborIdx = neighborRow * m_NrOfColumns + neighborCol;
			float connectionCost = CalculateConnectionCost(idx, neighborIdx);

			if (!Graph::ConnectionExists(idx, neighborIdx)
				&& connectionCost < 100000) //Extra check for different terrain types
				AddConnection(new GraphConnection(idx, neighborIdx, connectionCost));
		}
	}
}

int GridGraph::GetNodeIdAtPosition(const Vector2& pos) const
{
	int idx = invalid_node_id;

	if (pos.x < 0 || pos.y < 0)
	{
		return idx;
	}

	int r, c;

	c = int(pos.x / m_CellSize);
	r = int(pos.y / m_CellSize);

	if (!IsWithinBounds(c, r))
		return idx;

	return GetNodeId(c, r);
}

void GridGraph::AddConnectionsToAdjacentCells(int idx)
{
	auto [row, col] = GetRowAndColumn(idx);

	// Add connections in all directions, taking into account the dimensions of the grid
	AddConnectionsInDirections(idx, col, row, m_StraightDirections);

	if (m_IsConnectedDiagonally)
	{
		AddConnectionsInDirections(idx, col, row, m_DiagonalDirections);
	}

	OnGraphModified(false, true);
}

Vector2 Elite::GridGraph::GetNodePos(int nodeId) const
{
	auto [row, col] = GetRowAndColumn(nodeId);
	return CalculatePosition(col, row);
}


GraphNode* GridGraph::GetNodeAtPosition(const Vector2& pos) const
{
	return Graph::GetNode(GetNodeIdAtPosition(pos));
}




bool GridGraph::IsWithinBounds(int col, int row) const
{
	return (col >= 0 && col < m_NrOfColumns && row >= 0 && row < m_NrOfRows);
}

Vector2 GridGraph::CalculatePosition(int col, int row)const
{
	Vector2 cellCenterOffset = { m_CellSize * .5f, m_CellSize * .5f };
	return Vector2{ (float)col * m_CellSize, (float)row * m_CellSize } + cellCenterOffset;
}

float GridGraph::CalculateConnectionCost(int fromId, int toId) const
{

	float cost = m_DefaultCostStraight;

	Vector2 fromPos = Graph::GetNode(fromId)->GetPosition();
	Vector2 toPos = Graph::GetNode(toId)->GetPosition();

	if (int(fromPos.y) != int(toPos.y) &&
		int(fromPos.x) != int(toPos.x))
	{
		cost = m_DefaultCostDiagonal;
	}

	if (m_pCostCalculator != nullptr)
	{
		cost *= m_pCostCalculator->CalculateConnectionCost(static_cast<const Graph*>(this), fromId, toId);
	}

	return cost;
}