#pragma once

#include "../EliteGraph/EGraph.h"
namespace Elite
{
	
	class GraphNode;
	class ConnectionCostCalculator;

	class GridGraph : public Graph
	{

	public:

		GridGraph(int columns, int rows, int cellSize, bool isDirectionalGraph, bool isConnectedDiagonally, float costStraight = 1.f, float costDiagonal = 1.5, GraphNodeFactory* factory = nullptr, ConnectionCostCalculator* pCostCalculator =nullptr);
		virtual ~GridGraph();
		int GetRows() const { return m_NrOfRows; }
		int GetColumns() const { return m_NrOfColumns; }

		bool IsWithinBounds(int col, int row) const;
		int GetNodeId(int col, int row) const { return row * m_NrOfColumns + col; }

		using Graph::GetNode;
		GraphNode* GetNode(int col, int row) { return Graph::GetNode(GetNodeId(col, row)); }

		int GetNodeIdAtPosition(const Vector2& pos) const override;
		GraphNode* GetNodeAtPosition(const Vector2& pos) const override;

		void AddConnectionsToAdjacentCells(int col, int row) { AddConnectionsToAdjacentCells(GetNodeId(col, row)); }
		void AddConnectionsToAdjacentCells(int idx);
		Vector2 GetNodePos(int nodeId) const override;
		std::pair<int, int> GetRowAndColumn(int idx) const { return { idx / m_NrOfColumns, idx % m_NrOfColumns }; }

	private:

		int m_NrOfColumns;
		int m_NrOfRows;
		int m_CellSize;

		bool m_IsConnectedDiagonally;
		float m_DefaultCostStraight;
		float m_DefaultCostDiagonal;

		const std::vector<Vector2> m_StraightDirections = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
		const std::vector<Vector2> m_DiagonalDirections = { { 1, 1 }, { -1, 1 }, { -1, -1 }, { 1, -1 } };
	
		ConnectionCostCalculator* m_pCostCalculator = nullptr;

		void InitializeGrid();
		void AddConnectionsInDirections(int idx, int col, int row, std::vector<Vector2> directions);

		virtual float CalculateConnectionCost(int fromIdx, int toIdx) const;
		Vector2 CalculatePosition(int col, int row) const;
		
		friend class GraphRenderer;
	};
}