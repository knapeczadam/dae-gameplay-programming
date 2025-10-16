#pragma once
#include <algorithm>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
#include "EHeuristic.h"

namespace Elite
{

	class AStar
	{
	public:
		AStar(Graph* pGraph, Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord
		{
			GraphNode* pNode = nullptr;
			GraphConnection* pConnection = nullptr;
			float costSoFar = 0.f; // accumulated g-costs of all the connections leading up to this one
			float estimatedTotalCost = 0.f; // f-cost (= costSoFar + h-cost)

			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& pConnection == other.pConnection
					&& costSoFar == other.costSoFar
					&& estimatedTotalCost == other.estimatedTotalCost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return estimatedTotalCost < other.estimatedTotalCost;
			};
		};

		std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode);
		std::vector<GraphNode*> FindPathV2(GraphNode* pStartNode, GraphNode* pDestinationNode);

	private:
		float GetHeuristicCost(GraphNode* pStartNode, GraphNode* pEndNode) const;

		Graph* m_pGraph;
		Heuristic m_HeuristicFunction;
	};

	
}