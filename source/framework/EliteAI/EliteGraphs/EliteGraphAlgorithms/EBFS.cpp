#include "stdafx.h"
#include "EBFS.h"

#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphNode.h"
#include "../EliteGraph/EGraphConnection.h"

using namespace Elite;

BFS::BFS(Graph* pGraph)
	: m_pGraph(pGraph)
{
}

//Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<GraphNode*> BFS::FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode)
{
	std::vector<GraphNode*> path{};

	std::queue<GraphNode*> openList{}; // it's common to name this openList even though it's a queue
	std::map<GraphNode*, GraphNode*> closedList{};
	
	openList.push(pStartNode);
	closedList[pStartNode] = nullptr;
	while (not openList.empty())
	{
		GraphNode* currentNode = openList.front();
		openList.pop();

		if (currentNode == pDestinationNode)
		{
			break;
		}

		// get all neighbors of the current node
		auto connections = m_pGraph->GetConnectionsFromNode(currentNode);
		for (const auto & connection : connections)
		{
			GraphNode* nextNode { m_pGraph->GetNode(connection->GetToNodeId())};
			if (not closedList.contains(nextNode))
			{
				 openList.push(nextNode);
				 closedList[nextNode] = currentNode;
			}
		}
	}

	if (closedList.contains(pDestinationNode))
	{
		GraphNode* currentNode = pDestinationNode;
		while (currentNode != pStartNode)
		{
			path.push_back(currentNode);
			currentNode = closedList[currentNode];
		}
		path.push_back(pStartNode);
		std::reverse(path.begin(), path.end());
	}

	return path;
}