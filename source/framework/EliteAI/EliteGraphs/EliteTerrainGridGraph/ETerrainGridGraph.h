#pragma once
#include "framework/EliteAI/EliteGraphs/EliteGridGraph/EGridGraph.h"

namespace Elite
{
	class GraphNode;
	class TerrainGraphNode;

	class TerrainGridGraph : public GridGraph
	{
	public:
		TerrainGridGraph(int columns, int rows, int cellSize, bool isDirectionalGraph, bool isConnectedDiagonally, float costStraight = 1.f, float costDiagonal = 1.5);
		virtual ~TerrainGridGraph();
		
		void SetNodeTerrainType(int node, TerrainType type)const;
	};
}