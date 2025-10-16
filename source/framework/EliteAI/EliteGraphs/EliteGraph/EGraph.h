//*=================================================*/
// EGraph.h: Graph base class, used for 2D graphs
//*=================================================*/

#pragma once
#include "../EliteGraphUtilities/EGraphVisuals.h"
#include "EGraphNode.h"
#include "EGraphEnums.h"
#include "../EliteGraphNodeFactory/EGraphNodeFactory.h"

namespace Elite
{
	class GraphConnection;

	class Graph
	{
	public:
		Graph(bool isDirectional, GraphNodeFactory* pNodeFactory = nullptr);
		Graph(const Graph& other);
		virtual ~Graph();

		//Graph properties
		bool IsDirectional() const { return m_isDirectional; }
		int GetNextNodeId() const { return m_nextNodeId; }
		void Clear();
		int GetAmountOfConnections() { return m_amountConnections; }
		int GetAmountOfNodes() const { return m_amountNodes; }
		int GetAmountOfInactiveNodes() const { return m_pNodes.size() - m_amountNodes; }

		std::shared_ptr<Graph> Clone() const;

		//Nodes
		GraphNode* GetNode(int nodeId) const;
		bool IsNodeValid(int nodeId) const;
		int AddNode(GraphNode* pNode);
		void RemoveNode(int nodeId);
		const std::vector<GraphNode*>& GetAllNodes() const;

		//Connections
		void AddConnection(GraphConnection* pConnection);
		GraphConnection* GetConnection(int fromNodeId, int toNodeId) const;
		void RemoveConnection(int fromNodeId, int toNodeId);
		void RemoveConnection(GraphConnection* pConnection);
		void RemoveAllConnectionsWithNode(int nodeId);

		const std::vector<GraphConnection*>& GetConnectionsFromNode(int nodeId) const;
		const std::vector<GraphConnection*>& GetConnectionsFromNode(GraphNode* pNode) const { return GetConnectionsFromNode(pNode->GetId()); }
		GraphConnection* GetConnectionAtPosition(Vector2& position, float maxDist = 1.0f) const;

		void SetConnectionCostsToDistances();

		//Query nodes and connections
		int GetNodeIdAtPosition(const Vector2& position, float errorMargin) const;
		GraphNode* GetNodeAtPosition(const Vector2& position, float errorMargin) const;
		bool ConnectionExists(int fromNodeId, int toNodeId) const { return GetConnection(fromNodeId, toNodeId) != nullptr; }
		
		virtual Vector2 GetNodePos(int nodeId) const { auto pNode = GetNode(nodeId); return pNode == nullptr ? Vector2{ 0,0 } : pNode->GetPosition(); }
		virtual int GetNodeIdAtPosition(const Vector2& position) const { return GetNodeIdAtPosition(position, 1.0f); }
		virtual GraphNode* GetNodeAtPosition(const Vector2& position) const { return GetNodeAtPosition(position, 1.0f); }


	protected:
		virtual void OnGraphModified(bool nrOfNodesChanged, bool nrOfConnectionsChanged) {}
		void AddNodeAtIndex(GraphNode* pNode, int index);

		bool m_isDirectional;
		int m_nextNodeId{ 0 };
		std::vector<GraphNode*> m_pNodes;
		std::vector<std::vector<GraphConnection*>>m_pConnections;
		std::vector<GraphNode*> m_pActiveNodes;

		std::shared_ptr<GraphNodeFactory> m_pNodeFactory;

		GraphNode* CreateNode(const Vector2& pos) { return m_pNodeFactory == nullptr ? new GraphNode(pos) : m_pNodeFactory->CreateNode(pos); }
		GraphNode* CloneNode(const GraphNode& other) { return m_pNodeFactory == nullptr ? new GraphNode(other) : m_pNodeFactory->CloneNode(other); }
	private:
		int m_amountNodes{ 0 };
		int m_amountConnections{ 0 };

		void UpdateNextNodeIndex();
		void UpdateActiveNodes();
	};



}
