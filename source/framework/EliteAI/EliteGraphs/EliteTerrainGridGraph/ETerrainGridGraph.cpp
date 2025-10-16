#include "stdafx.h"

#include "ETerrainGridGraph.h"
#include "ETerrainGraphNode.h"
#include "../EliteGridGraph/EGridGraph.h"
#include "../EliteGraphNodeFactory/EGraphNodeFactory.h"
#include "ETerrainCostCalculator.h"

using namespace Elite;

TerrainGridGraph::TerrainGridGraph(int columns, int rows, int cellSize, bool isDirectionalGraph, bool isConnectedDiagonally, float costStraight, float costDiagonal)
	:GridGraph(columns, rows, cellSize,
		isDirectionalGraph, isConnectedDiagonally, costStraight, costDiagonal, 
		new GraphNodeFactoryTemplate<TerrainGraphNode>(), new TerrainCostCalculator())
{

}

TerrainGridGraph::~TerrainGridGraph()
{
}

void TerrainGridGraph::SetNodeTerrainType(int nodeId, TerrainType type) const
{
	auto node = reinterpret_cast<TerrainGraphNode*>(GetNode(nodeId));

	node->SetTerrainType(type);
}
