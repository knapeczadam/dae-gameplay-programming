#pragma once

namespace Elite
{
	class Graph;
	class GraphNode;

	class BFS
	{
	public:
		BFS(Graph* pGraph);

		std::vector<GraphNode*> FindPath(GraphNode* pStartNode, GraphNode* pDestinationNode);
	private:
		Graph* m_pGraph;
	};

}

