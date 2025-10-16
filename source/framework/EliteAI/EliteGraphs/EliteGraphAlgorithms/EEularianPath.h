#pragma once
#include <stack>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	inline std::string ToString(Eulerianity eulerianity) 
	{
		switch (eulerianity)
		{
		case Eulerianity::notEulerian:
			return "Not Eulerian";
		case Eulerianity::semiEulerian:
			return "Semi-Eulerian";
		case Eulerianity::eulerian:
			return "Eulerian";
		}
	}

	class EulerianPath
	{
	public:

		EulerianPath(Graph* pGraph);

		Eulerianity IsEulerian() const;
		std::vector<GraphNode*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// If the graph is not connected, there can be no Eulerian Trail
		if (not IsConnected()) return Eulerianity::notEulerian;


		// Count nodes with odd degree
		auto nodes = m_pGraph->GetAllNodes();
		int nrOfOddNodes = 0;
		for (auto node : nodes)
		{
			auto connections = m_pGraph->GetConnectionsFromNode(node);
			const int nrOfConnections = connections.size();
			if (nrOfConnections % 2 != 0)
			// if (nrOfConnections & 1)	
				++nrOfOddNodes;
		}

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (nrOfOddNodes > 2 or nrOfOddNodes == 1) return Eulerianity::notEulerian;


		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// An Euler trail can be made, but only starting and ending in these 2 nodes
		if (nrOfOddNodes == 2) return Eulerianity::semiEulerian;


		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;

	}

	// TODO: implement this function
	inline std::vector<GraphNode*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		auto path = std::vector<GraphNode*>();
		int nrOfNodes = graphCopy->GetAmountOfNodes();

		// Check if there can be an Euler path
		// If this graph is not eulerian, return the empty path
		eulerianity = IsEulerian();
		if (eulerianity == Eulerianity::notEulerian)
			return path;
		
		// Find a valid starting index for the algorithm
		int startIndex = 0;
		if (eulerianity == Eulerianity::semiEulerian)
		{
			for (const auto& node : graphCopy->GetAllNodes())
			{
				if (graphCopy->GetConnectionsFromNode(node).size() % 2 != 0)
				{
					startIndex = node->GetId();
					break;
				}
			}
		}
		else
		{
			startIndex = graphCopy->GetAllNodes()[0]->GetId();
		}
		

		// Start algorithm loop
		std::stack<int> nodeStack;
		nodeStack.push(startIndex);
		GraphNode* currentNode { m_pGraph->GetNode(startIndex) };
		while (not nodeStack.empty())
		{
			currentNode = m_pGraph->GetNode(nodeStack.top());
			if (graphCopy->GetConnectionsFromNode(currentNode).empty())
			{
				path.push_back(currentNode);
				nodeStack.pop();
			}
			else
			{
				const auto nextNode = m_pGraph->GetNode(graphCopy->GetConnectionsFromNode(currentNode)[0]->GetToNodeId());
				graphCopy->RemoveConnection(currentNode->GetId(), nextNode->GetId());
				nodeStack.push(nextNode->GetId());
			}
		}


		std::ranges::reverse(path); // reverses order of the path
		return path;
	}


	// TODO: implement this function
	inline void EulerianPath::VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex ) const
	{
		// mark the visited node
		visited[startIndex] = true;
		
		// recursively visit any valid connected nodes that were not visited before
		for (const auto& node : m_pGraph->GetConnectionsFromNode(startIndex))
		{
			if (not visited[node->GetToNodeId()])
				VisitAllNodesDFS(pNodes, visited, node->GetToNodeId());
		}
	}

	// TODO: implement this function
	inline bool EulerianPath::IsConnected() const
	{
		auto nodes = m_pGraph->GetAllNodes();
		if (nodes.size() == 0)
			return false;

		// find a valid starting node that has connections
		GraphNode* startNode = nullptr;
		for (const auto& node : nodes)
		{
			if (not m_pGraph->GetConnectionsFromNode(node).empty())
			{
				startNode = node;
				break;
			}
		}
		// if no valid node could be found, return false
		if (startNode == nullptr)
			return false;
		
		const auto nodeWithMaxId {std::ranges::max_element(nodes, [](const GraphNode* a, const GraphNode* b) { return a->GetId() < b->GetId(); } )};
		const int maxId = (*nodeWithMaxId)->GetId();
		
		// start a depth-first-search traversal from the node that has at least one connection
		std::vector<bool> visited(maxId + 1, false);
		VisitAllNodesDFS(nodes, visited, startNode->GetId());

		// if a node was never visited, this graph is not connected
		for (const auto& node : nodes)
		{
			if (not visited[node->GetId()])
				return false;
		}
		return true;
	}

}